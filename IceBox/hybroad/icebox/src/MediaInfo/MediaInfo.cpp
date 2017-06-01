/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license that can
 *  be found in the License.html file in the root of the source tree.
 */

//---------------------------------------------------------------------------
#include <vector>
#include <algorithm>
#include <iostream>
#include <ZenLib/Ztring.h>
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "MediaInfo.h"
#include "Core.h"
#include "TinyXml/tinyxml.h"

using namespace std;
using namespace MediaInfoNameSpace;
//---------------------------------------------------------------------------
namespace IBox {

int
MediaInfo::getInfo(const char* file, MiAudioInfoPtr sMi)
{
    //Localisation
    setlocale(LC_ALL, "");
    //MediaInfo::Option_Static(__T("CharSet"), __T(""));
    //MediaInfo::Option_Static(__T("LineSeparator"), __T("\n")); //Using sdtout
    
    //Configure MediaInfo core
    Core MI;
    MI.Menu_View_XML(); //Default to text with CLI.

    //Retrieve command line (mainly for Unicode)
    std::vector<MediaInfoNameSpace::String> argv;
    ZenLib::Ztring A;
    A.From_Local(file);
    argv.push_back(A);

    vector<String> List;
    List.push_back(argv[0]);
    
    //Parse files
    MI.Menu_File_Open_Files_Begin();
    MI.Menu_File_Open_Files_Continue(List[0]);

    //Output
    //STRINGOUT(MI.Inform_Get());
    ZenLib::Ztring Text = MI.Inform_Get();
    mediaInfoAnalyze(Text.To_Local(), sMi);

    return 0;
}
//---------------------------------------------------------------------------

void
MediaInfo::mediaInfoAnalyze(string info, MiAudioInfoPtr sMi)
{
    //cout  << info << endl;
    //return ;
    //const char * xmlFile = "aa.xml";
    TiXmlDocument doc;
    //return;
    doc.LoadFile(info.c_str(), info.size());

    TiXmlElement* rootElement = doc.RootElement();  //School元素
    if (rootElement == NULL)
        return;
    TiXmlElement* classElement = rootElement->FirstChildElement();  // Class元素
    if (classElement == NULL)
        return;
    TiXmlElement* studentElement = classElement->FirstChildElement();  //Students
    for (; studentElement != NULL; studentElement = studentElement->NextSiblingElement() ) {
        TiXmlAttribute* attributeOfStudent = studentElement->FirstAttribute();  //获得student的name属性
        //for (;attributeOfStudent != NULL; attributeOfStudent = attributeOfStudent->Next() ) {
        //    cout << attributeOfStudent->Name() << " aaa : " << attributeOfStudent->Value() << std::endl;
        //}

        TiXmlElement* studentContactElement = studentElement->FirstChildElement();//获得student的第一个联系方式
        for (; studentContactElement != NULL; studentContactElement = studentContactElement->NextSiblingElement() ) {
            string contactType = studentContactElement->Value();
            string contactValue = studentContactElement->GetText();
            if (contactType.find("Album___") != string::npos ) {                
                //cout << contactType  << " : " << contactValue << std::endl;
                sMi->mAlbum = contactValue;
            } else if (contactType.find("Performer___") != string::npos
                    && contactType.find("Album_Performer") == string::npos) {
                //cout << contactType  << " : " << contactValue << std::endl;
                sMi->mPerformer = contactValue;
            } else if (contactType.find("Duration___") != string::npos ) {
                //cout << contactType  << " : " << contactValue << std::endl;
                sMi->mDuration = contactValue;
            } else if (contactType.find("Bit_rate___") != string::npos ) {
                //cout << contactType  << " : " << contactValue << std::endl;
                sMi->mBitRate = contactValue;
            }

        }
    }
}

}
#if 0
int main(int argc, char* argv_ansi[])
{
    MiAudioInfoPtr sMi = new MiAudioInfo;
    MediaInfo mi;
    mi.getInfo(argv_ansi[1], sMi);
    cout << "Album     :" << sMi->mAlbum;
    cout << "\nPerformer :" << sMi->mPerformer;
    cout << "\nDuration  :" << sMi->mDuration;
    cout << "\nBit_rate  :" << sMi->mBitRate << endl;
    delete sMi;
    
    return 0;
}

#endif


