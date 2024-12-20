//*****************************************************************************
// MIT License
//
// Copyright (c) 2024 Alain Duron <duron.alain@gmail.com>
// Copyright (c) 2024 Quentin Quadrat <lecrapouille@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//*****************************************************************************

#include "ad_blocker.hpp"
#include "helper_log.hpp"

//------------------------------------------------------------------------------
AdBlocker::AdBlocker()
{
    GDCEF_DEBUG("");

    // Add common ad patterns
    const std::vector<std::string> default_patterns = {
        R"(.*doubleclick\.net.*)",
        R"(.*googlesyndication\.com.*)",
        R"(.*google-analytics\.com.*)",
        R"(.*adnxs\.com.*)",
        R"(.*advertising\.com.*)",
        R"(.*/ads/.*)",
        R"(.*/adserv.*)",
        R"(.*/banner.*)",
        R"(.*/analytics.*)",
        R"(.*/tracker.*)"};

    for (const auto& pattern : default_patterns)
    {
        addPattern(pattern);
    }
}

//------------------------------------------------------------------------------
void AdBlocker::enable(bool enable)
{
    m_enabled = enable;
}

//------------------------------------------------------------------------------
bool AdBlocker::addPattern(const std::string& pattern)
{
    GDCEF_DEBUG("Adding ad blocking pattern: " << pattern);
    try
    {
        m_patterns.push_back(std::regex(pattern, std::regex::icase));
        return true;
    }
    catch (const std::regex_error& e)
    {
        GDCEF_ERROR("Invalid ad blocking pattern: " << pattern);
        return false;
    }
}

//------------------------------------------------------------------------------
CefResourceRequestHandler::ReturnValue
AdBlocker::OnBeforeResourceLoad(CefRefPtr<CefBrowser> browser,
                                CefRefPtr<CefFrame> frame,
                                CefRefPtr<CefRequest> request,
                                CefRefPtr<CefCallback> callback)
{
    std::string url = request->GetURL().ToString();
    if (m_enabled)
    {
        for (const auto& pattern : m_patterns)
        {
            if (std::regex_match(url, pattern))
            {
                GDCEF_DEBUG("Blocked ad URL: " << url);
                return RV_CANCEL; // Block the request
            }
        }
    }

    GDCEF_DEBUG("Allowed ad URL: " << url);
    return RV_CONTINUE; // Allow the request
}