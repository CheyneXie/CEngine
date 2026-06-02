/**
 * @file WindowsFileDialog.ixx
 * @brief Windows文件对话框
 * @version 1.0
 * @author ChatGPT-4o, Chaim
 * @date 2024/10/17
 */

module;
#ifdef _WIN32
#include <shobjidl_core.h>
#endif
export module CEngine.Utils.WindowsFileDialog;
#ifdef _WIN32
import std;
namespace fs = std::filesystem;

namespace CEngine::Utils::WindowsFileDialog {
    // Utility function to initialize COM library
    void initializeCOM() {
        if (const HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE); FAILED(hr)) {
            throw std::runtime_error("Failed to initialize COM library.");
        }
    }

    // Utility function to create file dialog
    IFileDialog *createFileDialog(const CLSID clsidDialogType) {
        IFileDialog *pfd = nullptr;
        if (const HRESULT hr = CoCreateInstance(clsidDialogType, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd)); FAILED(hr)) {
            throw std::runtime_error("Failed to create file dialog.");
        }
        return pfd;
    }

    /**
     * 打开文件对话框选择文件打开
     * @remark 取消返回空
     * @param file_filter 文件名过滤器，如<code>{ {L"Json", L"*.json"}, {L"TXT", L"*.txt"}}</code>
     * @return 返回文件路径
     */
    export std::filesystem::path OpenFile(const std::vector<std::pair<const wchar_t *, const wchar_t *> > &file_filter = {}) {
        initializeCOM();
        IFileDialog *pfd = createFileDialog(CLSID_FileOpenDialog);
        std::vector<COMDLG_FILTERSPEC> filterSpec(file_filter.size());
        for (int i = 0; i < file_filter.size(); i++) {
            filterSpec[i].pszName = file_filter[i].first;
            filterSpec[i].pszSpec = file_filter[i].second;
        }
        pfd->SetFileTypes(static_cast<UINT>(filterSpec.size()), filterSpec.data());
        if (HRESULT hr = pfd->Show(nullptr); SUCCEEDED(hr)) {
            IShellItem *pItem = nullptr;
            hr = pfd->GetResult(&pItem);
            if (SUCCEEDED(hr)) {
                PWSTR pszFilePath = nullptr;
                hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
                if (SUCCEEDED(hr)) {
                    std::filesystem::path filePath = pszFilePath;
                    CoTaskMemFree(pszFilePath);
                    pItem->Release();
                    pfd->Release();
                    CoUninitialize();
                    return filePath;
                }
                pItem->Release();
            }
        }
        pfd->Release();
        CoUninitialize();
        return "";
    }

    /**
     * 打开文件对话框选择文件保存
     * @remark 取消返回空
     * @param file_filter 文件名过滤器，如<code>{ {L"Json", L"*.json"}, {L"TXT", L"*.txt"}}</code>
     * @return 返回文件路径
     */
    export std::filesystem::path SaveFile(const std::vector<std::pair<const wchar_t *, const wchar_t *> > &file_filter = {}) {
        initializeCOM();
        IFileDialog *pfd = createFileDialog(CLSID_FileSaveDialog);
        std::vector<COMDLG_FILTERSPEC> filterSpec(file_filter.size());
        for (int i = 0; i < file_filter.size(); i++) {
            filterSpec[i].pszName = file_filter[i].first;
            filterSpec[i].pszSpec = file_filter[i].second;
        }
        pfd->SetFileTypes(static_cast<UINT>(filterSpec.size()), filterSpec.data());
        // pfd->SetDefaultExtension(file_filter[0].second);
        if (HRESULT hr = pfd->Show(nullptr); SUCCEEDED(hr)) {
            IShellItem *pItem = nullptr;
            hr = pfd->GetResult(&pItem);
            if (SUCCEEDED(hr)) {
                PWSTR pszFilePath = nullptr;
                hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
                if (SUCCEEDED(hr)) {
                    std::filesystem::path filePath = pszFilePath;
                    CoTaskMemFree(pszFilePath);
                    pItem->Release();
                    pfd->Release();
                    CoUninitialize();
                    return filePath;
                }
                pItem->Release();
            }
        }
        pfd->Release();
        CoUninitialize();
        return "";
    }

    /**
     * 选择文件夹
     * @remark 取消返回空
     * @return 文件夹路径
     */
    export std::filesystem::path SelectFolder() {
        initializeCOM();

        IFileDialog *pfd = createFileDialog(CLSID_FileOpenDialog);
        DWORD dwOptions;
        pfd->GetOptions(&dwOptions);
        pfd->SetOptions(dwOptions | FOS_PICKFOLDERS);

        if (HRESULT hr = pfd->Show(nullptr); SUCCEEDED(hr)) {
            IShellItem *pItem = nullptr;
            hr = pfd->GetResult(&pItem);
            if (SUCCEEDED(hr)) {
                PWSTR pszFolderPath = nullptr;
                hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFolderPath);
                if (SUCCEEDED(hr)) {
                    std::filesystem::path folderPath = pszFolderPath;
                    CoTaskMemFree(pszFolderPath);
                    pItem->Release();
                    pfd->Release();
                    CoUninitialize();
                    return folderPath;
                }
                pItem->Release();
            }
        }
        pfd->Release();
        CoUninitialize();
        return "";
    }
}
#endif