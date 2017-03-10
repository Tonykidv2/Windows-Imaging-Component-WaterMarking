#pragma once
//#include "../Common/stdafx.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <algorithm>
#include <d2d1.h>
#include <ShObjIdl.h>
#include <dwrite.h>
#include <wincodecsdk.h>

//#include "../Common/resource.h"
#include <wrl/client.h>
#include <wincodec.h>
#include <string>
#include <sstream>
using namespace std;
using namespace Microsoft::WRL;
//using namespace Hilo::Direct2DHelpers;
#include "HelperFunctions.h"

#define SafeRelease(X) {if(X) {X->Release(); X = nullptr;}}