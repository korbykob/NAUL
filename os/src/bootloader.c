#include <bootloader.h>
#include <efi.h>
#include <efilib.h>
#include <serial.h>
#include <allocator.h>
#include <kernel.h>

typedef struct
{
    char signature[4];
    uint32_t length;
    uint8_t revision;
    uint8_t checksum;
    char oemId[6];
    char oemTableID[8];
    uint32_t oemRevision;
    uint32_t creatorID;
    uint32_t creatorRevision;
} __attribute__ ((packed)) AcpiSdtHeader;
typedef struct
{
    AcpiSdtHeader header;
    AcpiSdtHeader* entries[];
} __attribute__ ((packed)) Xsdt;
typedef struct
{
    char signature[8];
    uint8_t checksum;
    char oemId[6];
    uint8_t revision;
    uint32_t rsdtAddress;
    uint32_t length;
    Xsdt* xsdt;
    uint8_t extendedChecksum;
    uint8_t reserved[3];
} __attribute__ ((packed)) Xsdp;
typedef struct 
{
    AcpiSdtHeader header;
    uint32_t blockId;
    uint32_t gas;
    uint64_t address;
    uint8_t number;
    uint16_t minimum;
    uint8_t protection;
} __attribute__((packed)) Hpet;

Info information = 
{
    0,
    0,
    0,
    0,
    0,
    0
};

EFI_FILE_INFO* openFolder(EFI_FILE_HANDLE folder)
{
    EFI_STATUS status = EFI_SUCCESS;
    EFI_FILE_INFO *buffer = NULL;
    UINTN bufferSize = SIZE_OF_EFI_FILE_INFO + 200;
    BOOLEAN end = FALSE;
    while (GrowBuffer(&status, (void**)&buffer, bufferSize))
    {
        status = uefi_call_wrapper(folder->Read, 3, folder, &bufferSize, buffer);
        if (bufferSize == 0)
        {
            end = TRUE;
            break;
        }
    }
    if (end)
    {
        FreePool(buffer);
        return NULL;
    }
    return buffer;
}

void parseFolder(EFI_FILE_HANDLE fs, const CHAR16* name, void (*found)(EFI_FILE_HANDLE fs, const CHAR16* name))
{
    EFI_FILE_HANDLE folder = NULL;
    uefi_call_wrapper(fs->Open, 5, fs, &folder, name, EFI_FILE_MODE_READ, 0);
    EFI_FILE_INFO* info = NULL;
    while ((info = openFolder(folder)))
    {
        if (StrCmp(info->FileName, u".") != 0 && StrCmp(info->FileName, u"..") != 0)
        {
            UINTN pathLength = StrLen(name);
            UINTN nameLength = StrLen(info->FileName);
            CHAR16* fullName = AllocatePool((pathLength + nameLength + 2) * sizeof(CHAR16));
            RtCopyMem(fullName, name, pathLength * 2);
            fullName[pathLength] = u'\\';
            RtCopyMem(fullName + pathLength + 1, info->FileName, nameLength * sizeof(CHAR16));
            fullName[pathLength + nameLength + 1] = u'\0';
            if (info->Attribute & EFI_FILE_DIRECTORY)
            {
                parseFolder(fs, fullName, found);
            }
            found(fs, fullName);
            FreePool(fullName);
        }
        FreePool(info);
    }
    uefi_call_wrapper(folder->Close, 1, folder);
}

void countFiles(EFI_FILE_HANDLE fs, const CHAR16* name)
{
    information.fileCount++;
}

void addFiles(EFI_FILE_HANDLE fs, const CHAR16* name)
{
    UINTN nameLength = StrLen(name);
    information.fileData[information.fileCount].name = AllocatePool(nameLength);
    for (UINTN i = 0; i < nameLength; i++)
    {
        information.fileData[information.fileCount].name[i] = name[i + 1] == u'\\' ? u'/' : name[i + 1];
    }
    EFI_FILE_HANDLE file = NULL;
    uefi_call_wrapper(fs->Open, 5, fs, &file, name, EFI_FILE_MODE_READ, 0);
    EFI_FILE_INFO* info = LibFileInfo(file);
    if (!(info->Attribute & EFI_FILE_DIRECTORY))
    {
        information.fileData[information.fileCount].size = info->FileSize;
        information.fileData[information.fileCount].data = AllocatePool(info->FileSize);
        uefi_call_wrapper(file->Read, 3, file, &info->FileSize, information.fileData[information.fileCount].data);
    }
    else
    {
        information.fileData[information.fileCount].size = 0;
        information.fileData[information.fileCount].data = NULL;
    }
    FreePool(info);
    information.fileCount++;
}

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable)
{
    __asm__ volatile ("movq %%cr4, %%rax; xorq $0x40000, %%rax; movq %%rax, %%cr4; xorq %%rcx, %%rcx; xgetbv; orl $7, %%eax; xsetbv" : : : "%rdx", "%rcx", "%rax");
    InitializeLib(ImageHandle, SystemTable);
    serialPrint("Locating GOP protocol");
    EFI_GRAPHICS_OUTPUT_PROTOCOL* GOP = NULL;
    LibLocateProtocol(&GraphicsOutputProtocol, (void**)&GOP);
    serialPrint("Resetting GOP");
    uefi_call_wrapper(GOP->SetMode, 2, GOP, 0);
    uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 0, 0);
    serialPrint("Disabling watchdog timer");
    uefi_call_wrapper(BS->SetWatchdogTimer, 4, 0, 0, 0, NULL);
    serialPrint("Displaying resolution prompt");
    Print(u"Use the up and down arrow keys to move.\nPress enter to select and boot using the selected resolution.\n\nPlease select a resolution:\n");
    for (UINT32 i = 0; i < GOP->Mode->MaxMode; i++)
    {
        UINTN size = 0;
        EFI_GRAPHICS_OUTPUT_MODE_INFORMATION* info = NULL;
        uefi_call_wrapper(GOP->QueryMode, 4, GOP, i, &size, &info);
        Print(u"%dx%d\n", info->HorizontalResolution, info->VerticalResolution);
    }
    serialPrint("Waiting for user to choose resolution");
    UINT32 selected = 0;
    EFI_INPUT_KEY pressed = { 1, u'\0' };
    while (pressed.UnicodeChar != '\r')
    {
        if (pressed.ScanCode == 1 || pressed.ScanCode == 2)
        {
            uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 10, selected + 4);
            Print(u"   ");
            if (pressed.ScanCode == 1 && selected != 0)
            {
                selected--;
            }
            else if (pressed.ScanCode == 2 && selected != GOP->Mode->MaxMode - 1)
            {
                selected++;
            }
            uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 10, selected + 4);
            Print(u"<--");
        }
        WaitForSingleEvent(ST->ConIn->WaitForKey, 0);
        uefi_call_wrapper(ST->ConIn->ReadKeyStroke, 2, ST->ConIn, &pressed);
    }
    serialPrint("Switching GOP mode");
    uefi_call_wrapper(GOP->SetMode, 2, GOP, selected);
    serialPrint("Searching tables");
    EFI_GUID guid = ACPI_20_TABLE_GUID;
    for (uint64_t i = 0; i < ST->NumberOfTableEntries; i++)
    {
        if (CompareGuid(&ST->ConfigurationTable[i].VendorGuid, &guid) == 0)
        {
            serialPrint("Found ACPI 2.0 table");
            Xsdt* xsdt = ((Xsdp*)ST->ConfigurationTable[i].VendorTable)->xsdt;
            serialPrint("Searching ACPI table");
            for (uint32_t table = 0; table < (xsdt->header.length - sizeof(AcpiSdtHeader)) / sizeof(AcpiSdtHeader*); table++)
            {
                if (strncmpa(xsdt->entries[table]->signature, "HPET", 4) == 0)
                {
                    serialPrint("Found HPET");
                    information.hpetAddress = ((Hpet*)xsdt->entries[table])->address;
                    break;
                }
            }
            break;
        }
    }
    serialPrint("Locating LIP protocol");
    EFI_LOADED_IMAGE* image = NULL;
    uefi_call_wrapper(BS->HandleProtocol, 3, ImageHandle, &LoadedImageProtocol, &image);
    serialPrint("Opening root file system");
    EFI_FILE_HANDLE root = LibOpenRoot(image->DeviceHandle);
    serialPrint("Counting files");
    parseFolder(root, u".", countFiles);
    serialPrint("Allocating room for files");
    information.fileData = AllocatePool(sizeof(InitFile) * information.fileCount);
    information.fileCount = 0;
    serialPrint("Reading files");
    parseFolder(root, u".", addFiles);
    UINTN entries = 0;
    UINTN key = 0;
    UINTN size = 0;
    UINT32 version = 0;
    serialPrint("Reading memory map");
    uint8_t* map = (uint8_t*)LibMemoryMap(&entries, &key, &size, &version);
    uint64_t memory = 0;
    uint64_t memorySize = 0;
    for (UINTN i = 0; i < entries; i++)
    {
        EFI_MEMORY_DESCRIPTOR* iterator = (EFI_MEMORY_DESCRIPTOR*)(map + i * size);
        if (iterator->Type == EfiConventionalMemory || iterator->Type == EfiBootServicesCode || iterator->Type == EfiBootServicesData)
        {
            uint64_t keySize = iterator->NumberOfPages * EFI_PAGE_SIZE;
            if (keySize > memorySize)
            {
                memory = iterator->PhysicalStart;
                memorySize = keySize;
            }
        }
    }
    initAllocator(memory + memorySize);
    serialPrint("Marking unusable memory");
    for (UINTN i = 0; i < entries; i++)
    {
        EFI_MEMORY_DESCRIPTOR* iterator = (EFI_MEMORY_DESCRIPTOR*)(map + i * size);
        if (iterator->Type != EfiConventionalMemory && iterator->Type != EfiBootServicesCode && iterator->Type != EfiBootServicesData)
        {
            markUnusable(iterator->PhysicalStart, iterator->PhysicalStart + iterator->NumberOfPages * EFI_PAGE_SIZE);
        }
    }
    serialPrint("Exiting boot services");
    uefi_call_wrapper(BS->ExitBootServices, 2, ImageHandle, key);
    serialPrint("Setting up information");
    information.framebuffer = (uint32_t*)GOP->Mode->FrameBufferBase;
    information.width = GOP->Mode->Info->HorizontalResolution;
    information.height = GOP->Mode->Info->VerticalResolution;
    serialPrint("Entering kernel");
    __asm__ volatile ("xorq %rbp, %rbp");
    kernel();
    while (TRUE);
    return EFI_SUCCESS;
}
