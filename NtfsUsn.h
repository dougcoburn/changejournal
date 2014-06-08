#include <Windows.h>
#include <string>
#include <vector>
#include <map>

struct USN_CPP {
    DWORDLONG ParentFileReferenceNumber;
    std::wstring FileName;
};

class NtfsUsn {
private:
    std::wstring volumeLabel;
    std::vector<USN_CPP> volumeFiles;
    std::map<DWORDLONG, USN_CPP> volumeFolders;

    std::wstring GetPathFromFolderReferenceNumber(DWORDLONG folderReferenceNumber);
public:
    NtfsUsn(std::wstring volumeLabel);
    void initFromUsn();
    std::vector<std::wstring> getAllFiles();
};