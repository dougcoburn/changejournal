#include "NtfsUsn.h"
#include "jump/handle.hpp"

#define BUF_LEN 0x10000

NtfsUsn::NtfsUsn(std::wstring volumeName) 
        : volumeLabel(volumeLabel) {
}

std::wstring NtfsUsn::GetPathFromFolderReferenceNumber(DWORDLONG refNumber) {
    std::wstring retval;
    auto p = volumeFolders.end();
    while ((p = volumeFolders.find(refNumber)) != end(volumeFolders)) {
        retval = p->second.FileName + L"\\" + retval;
        if (refNumber == p->second.ParentFileReferenceNumber) {
            break;
        }
        refNumber = p->second.ParentFileReferenceNumber;
    }
    return retval;
}

void NtfsUsn::initFromUsn() {
    jump::Handle hVol(CreateFileW(volumeLabel.c_str(),
            GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL,
            OPEN_EXISTING,
            0,
            NULL));

    if (!hVol) {
        printf("CreateFile failed (%d)\n", GetLastError());
        return;
    }

    USN_JOURNAL_DATA JournalData = { 0 };
    DWORD dwRetBytes = 0;
    if (!DeviceIoControl(
            hVol.get(),
            FSCTL_QUERY_USN_JOURNAL,
            NULL,
            0,
            &JournalData,
            sizeof(JournalData),
            &dwRetBytes,
            NULL)) {
        printf("Query journal failed (%d)\n", GetLastError());
        return;
    }

    MFT_ENUM_DATA EnumData = { 0, 0, JournalData.NextUsn, 2, 2 };

    std::vector<byte> Buffer(BUF_LEN);

    while (DeviceIoControl(hVol.get(),
        FSCTL_ENUM_USN_DATA,
        &EnumData,
        sizeof(EnumData),
        &(Buffer[0]),
        BUF_LEN,
        &dwRetBytes,
        NULL)) {

        // Update starting USN for next call
        EnumData.StartFileReferenceNumber = *((DWORDLONG*)&Buffer[0]);

        // We have consumed the next reference number from the returned buffer
        dwRetBytes -= sizeof(DWORDLONG);

        // The first Record in the buffer
        PUSN_RECORD UsnRecord = (PUSN_RECORD)(&Buffer[sizeof(DWORDLONG)]);

        // This loop could go on for a long time, given the current buffer size.
        while (dwRetBytes > 0) {

            if (UsnRecord->FileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                    volumeFolders[UsnRecord->FileReferenceNumber] = USN_CPP {
                    UsnRecord->ParentFileReferenceNumber,
                    std::wstring(UsnRecord->FileName, UsnRecord->FileNameLength / 2)
                };
            } else {
                volumeFiles.emplace_back(USN_CPP {
                    UsnRecord->ParentFileReferenceNumber,
                    std::wstring(UsnRecord->FileName, UsnRecord->FileNameLength / 2)
                });
            }

            dwRetBytes -= UsnRecord->RecordLength;

            // Find the next record
            UsnRecord = (PUSN_RECORD)(((PBYTE)UsnRecord) + UsnRecord->RecordLength);
        }
    }
}

std::vector<std::wstring> NtfsUsn::getAllFiles() {
    std::vector<std::wstring> retval;
    for (auto file = begin(volumeFiles); file != end(volumeFiles); file++) {
        retval.emplace_back(volumeLabel + GetPathFromFolderReferenceNumber(file->ParentFileReferenceNumber) + file->FileName);
    }
    return retval;
}
