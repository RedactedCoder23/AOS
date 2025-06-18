/*
 * AOS — EFI Loader
 * (c) 2025 RedactedCoder23
 * Purpose: Minimal UEFI loader for kernel and initramfs
 */
#include <efi.h>
#include <efilib.h>

EFI_STATUS efi_main(EFI_HANDLE image, EFI_SYSTEM_TABLE *systab) {
    InitializeLib(image, systab);

    EFI_STATUS status;
    EFI_LOADED_IMAGE *loaded_image = NULL;
    EFI_GUID li_guid = LOADED_IMAGE_PROTOCOL;
    status = uefi_call_wrapper(systab->BootServices->HandleProtocol, 3,
                               image, &li_guid, (void **)&loaded_image);
    if (EFI_ERROR(status)) {
        Print(L"HandleProtocol: %r\n", status);
        return status;
    }

    EFI_DEVICE_PATH *kernel_path = FileDevicePath(loaded_image->DeviceHandle,
                                                  L"\\kernel.bin");
    EFI_HANDLE kernel_handle = NULL;
    status = uefi_call_wrapper(systab->BootServices->LoadImage, 6,
                               FALSE, image, kernel_path, NULL, 0,
                               &kernel_handle);
    if (EFI_ERROR(status)) {
        Print(L"Failed to load kernel.bin: %r\n", status);
        return status;
    }

    EFI_DEVICE_PATH *initrd_path = FileDevicePath(loaded_image->DeviceHandle,
                                                  L"\\initramfs.img");
    EFI_HANDLE initrd_handle = NULL;
    status = uefi_call_wrapper(systab->BootServices->LoadImage, 6,
                               FALSE, image, initrd_path, NULL, 0,
                               &initrd_handle);
    if (!EFI_ERROR(status))
        uefi_call_wrapper(systab->BootServices->StartImage, 3,
                          initrd_handle, NULL, NULL);

    Print(L"Starting kernel...\n");
    status = uefi_call_wrapper(systab->BootServices->StartImage, 3,
                               kernel_handle, NULL, NULL);
    return status;
}
