#include "keybind.hpp"

struct camera_position_s {
	fvector location{};
	fvector rotation{};
	float fov{};
};
inline camera_position_s camera_postion{};

#define deg2Rad(angleDegrees) ((angleDegrees) * M_PI / 180.0)
#define rad2Deg(angleRadians) ((angleRadians) * 180.0 / M_PI)

struct FMinimalViewInfo
{
	FMinimalViewInfo() : Location(), Rotation(), FOV(FOV) {}
	FMinimalViewInfo(fvector Location, fvector Rotation, float FOV) : Location(Location), Rotation(Rotation), FOV(FOV) {}

	fvector Location, Rotation;
	float FOV;
};

inline FMinimalViewInfo m_camera_information{};

typedef struct _D3DMATRIX1 {
	union {
		struct {
			double _11, _12, _13, _14;
			double _21, _22, _23, _24;
			double _31, _32, _33, _34;
			double _41, _42, _43, _44;

		};
		double m[4][4];
	};
} D3DMATRIX1;

typedef struct D3DXMATRIX1 {
	DOUBLE _ij;
} D3DXMATRIX1, * LPD3DXMATRIX1;

struct FPlane : public fvector
{
	double W;

	FPlane() : W(0) { } //new 
	FPlane(double W) : W(W) { } //new
};

struct FMatrix
{
	double m[4][4]; //new
	FPlane XPlane, YPlane, ZPlane, WPlane;

	FMatrix() : XPlane(), YPlane(), ZPlane(), WPlane() { }
	FMatrix(FPlane XPlane, FPlane YPlane, FPlane ZPlane, FPlane WPlane)
		: XPlane(XPlane), YPlane(YPlane), ZPlane(ZPlane), WPlane(WPlane) { }

	FMatrix MatrixMultiplication(FMatrix Other)
	{
		FMatrix Copy = {};

		Copy.WPlane.x = this->WPlane.x * Other.XPlane.x + this->WPlane.y * Other.YPlane.x + this->WPlane.z * Other.ZPlane.x + this->WPlane.W * Other.WPlane.x;
		Copy.WPlane.y = this->WPlane.x * Other.XPlane.y + this->WPlane.y * Other.YPlane.y + this->WPlane.z * Other.ZPlane.y + this->WPlane.W * Other.WPlane.y;
		Copy.WPlane.z = this->WPlane.x * Other.XPlane.z + this->WPlane.y * Other.YPlane.z + this->WPlane.z * Other.ZPlane.z + this->WPlane.W * Other.WPlane.z;
		Copy.WPlane.W = this->WPlane.x * Other.XPlane.W + this->WPlane.y * Other.YPlane.W + this->WPlane.z * Other.ZPlane.W + this->WPlane.W * Other.WPlane.W;

		return Copy;
	}

	static FMatrix RotatorToMatrix(fvector Rotation)
	{
		auto ReturnValue = FMatrix();

		auto Pitch = (Rotation.x * 3.14159265358979323846 / 180);
		auto Yaw = (Rotation.y * 3.14159265358979323846 / 180);
		auto Roll = (Rotation.z * 3.14159265358979323846 / 180);

		auto sp = sin(Pitch);
		auto cp = cos(Pitch);
		auto sy = sin(Yaw);
		auto cy = cos(Yaw);
		auto sr = sin(Roll);
		auto cr = cos(Roll);

		ReturnValue.XPlane.x = cp * cy;
		ReturnValue.XPlane.y = cp * sy;
		ReturnValue.XPlane.z = sp;

		ReturnValue.YPlane.x = sr * sp * cy - cr * sy;
		ReturnValue.YPlane.y = sr * sp * sy + cr * cy;
		ReturnValue.YPlane.z = -sr * cp;

		ReturnValue.ZPlane.x = -(cr * sp * cy + sr * sy);
		ReturnValue.ZPlane.y = cy * sr - cr * sp * sy;
		ReturnValue.ZPlane.z = cr * cp;

		ReturnValue.WPlane.W = 0x1;

		return ReturnValue;
	}
};

//bool is_valid(const uint64_t adress)
//{
//	if (adress <= 0x400000 || adress == 0xCCCCCCCCCCCCCCCC || reinterpret_cast<void*>(adress) == nullptr || adress >
//		0x7FFFFFFFFFFFFFFF) {
//		return false;
//	}
//	return true;
//}

uintptr_t viewStates;
uintptr_t viewMatrices;
D3DMATRIX1 invProjMatrix;

inline std::wstring MBytesToWString(const char* lpcszString)
{

		int len = strlen(lpcszString);
	int unicodeLen = ::MultiByteToWideChar(CP_ACP, 0, lpcszString, -1, NULL, 0);
	wchar_t* pUnicode = new wchar_t[unicodeLen + 1];
	memset(pUnicode, 0, (unicodeLen + 1) * sizeof(wchar_t));
	::MultiByteToWideChar(CP_ACP, 0, lpcszString, -1, (LPWSTR)pUnicode, unicodeLen);
	std::wstring wString = (wchar_t*)pUnicode;
	delete[] pUnicode;
	return wString;
}
inline std::string WStringToUTF8(const wchar_t* lpwcszWString)
{
	char* pElementText;
	int iTextLen = ::WideCharToMultiByte(CP_UTF8, 0, (LPWSTR)lpwcszWString, -1, NULL, 0, NULL, NULL);
	pElementText = new char[iTextLen + 1];
	memset((void*)pElementText, 0, (iTextLen + 1) * sizeof(char));
	::WideCharToMultiByte(CP_UTF8, 0, (LPWSTR)lpwcszWString, -1, pElementText, iTextLen, NULL, NULL);
	std::string strReturn(pElementText);
	delete[] pElementText;
	return strReturn;
}

inline void DrawString(float fontSize, int x, int y, ImColor color, bool bCenter, bool stroke, const char* pText, ...)
{
	va_list va_alist;
	char buf[128] = { 0 };
	va_start(va_alist, pText);
	_vsnprintf_s(buf, sizeof(buf), pText, va_alist);
	va_end(va_alist);
	std::string text = WStringToUTF8(MBytesToWString(buf).c_str());
	if (bCenter)
	{
		ImVec2 textSize = ImGui::CalcTextSize(text.c_str());
		x = x - textSize.x / 4;
		y = y - textSize.y;
	}
	if (stroke)
	{
		ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), fontSize, ImVec2(x + 1, y + 1), ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 1)), text.c_str());
		ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), fontSize, ImVec2(x - 1, y - 1), ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 1)), text.c_str());
		ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), fontSize, ImVec2(x + 1, y - 1), ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 1)), text.c_str());
		ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), fontSize, ImVec2(x - 1, y + 1), ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 1)), text.c_str());
	}
	ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), fontSize, ImVec2(x, y), ImColor(color), text.c_str());
}

__forceinline void _DrawText(ImFont* Font, ImVec2 pos, ImU32 color, const char* text, float size = 18.0f, bool outline = false)
{
	ImDrawList* p = ImGui::GetBackgroundDrawList();
	if (outline) {
		ImVec2 text_size = Font->CalcTextSizeA(size, FLT_MAX, 0.0f, text);
		ImU32 outline_color = ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0));
		p->AddText(Font, size, ImVec2((pos.x) + 1, (pos.y + text_size.y) + 1), outline_color, text);
		p->AddText(Font, size, ImVec2((pos.x) - 1, (pos.y + text_size.y) - 1), outline_color, text);
		p->AddText(Font, size, ImVec2((pos.x) + 1, (pos.y + text_size.y) - 1), outline_color, text);
		p->AddText(Font, size, ImVec2((pos.x) - 1, (pos.y + text_size.y) + 1), outline_color, text);
	}

	p->AddText(Font, size, pos, color, text);
}

inline void DrawFilledRect(int x, int y, int w, int h, ImU32 color, bool round = false)
{

	if (round)
		ImGui::GetForegroundDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), color, ImDrawCornerFlags_All, 30);
	else
		ImGui::GetForegroundDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), color, 0, 0);
}

std::string string_To_UTF8(const std::string& str)
{
	int nwLen = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);

	wchar_t* pwBuf = new wchar_t[nwLen + 1];
	ZeroMemory(pwBuf, nwLen * 2 + 2);

	::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), pwBuf, nwLen);

	int nLen = ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL);

	char* pBuf = new char[nLen + 1];
	ZeroMemory(pBuf, nLen + 1);

	::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

	std::string retStr(pBuf);

	delete[]pwBuf;
	delete[]pBuf;

	pwBuf = NULL;
	pBuf = NULL;

	return retStr;
}

void DrawNewTextCustomWithOutline(int x, int y, int R, int G, int B, int A, const char* str)
{
	std::string utf_8_1 = std::string(str);
	std::string utf_8_2 = string_To_UTF8(utf_8_1);
	float outlineThickness = 1.5f; // Adjust the outline thickness as needed

	// Draw the text with black outline
	ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), 18.0f, ImVec2(x - outlineThickness, y), ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 0.0f, 0.0f, A / 255.0f)), utf_8_2.c_str());
	ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), 18.0f, ImVec2(x + outlineThickness, y), ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 0.0f, 0.0f, A / 255.0f)), utf_8_2.c_str());
	ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), 18.0f, ImVec2(x, y - outlineThickness), ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 0.0f, 0.0f, A / 255.0f)), utf_8_2.c_str());
	ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), 18.0f, ImVec2(x, y + outlineThickness), ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 0.0f, 0.0f, A / 255.0f)), utf_8_2.c_str());

	// Draw the main text with the specified color
	ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), 18.0f, ImVec2(x, y), ImGui::ColorConvertFloat4ToU32(ImVec4(R / 255.0f, G / 255.0f, B / 255.0f, A / 255.0f)), utf_8_2.c_str());
}

void TwoD(int x, int y, int w, int h, ImColor color, int thickness) {
	ImDrawList* Drawlist = ImGui::GetBackgroundDrawList();

	Drawlist->AddRect(ImVec2(x, y), ImVec2(x + w, y + h), color, 0.0f, 0, thickness);
}

inline void DrawCorneredBox(int x, int y, int w, int h, const ImU32& color, int thickness, int borderPx, bool outlined) {
	int cornerBoxLength = w / 3;
	int cornerBoxHeight = h / 5;
	DrawFilledRect(x + borderPx, y, w, borderPx, ImColor(10, 10, 10, 180));


	if (outlined) {
		
		ImGui::GetForegroundDrawList()->AddLine(ImVec2(x + cornerBoxLength, y), ImVec2(x, y), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), thickness * 2);
		ImGui::GetForegroundDrawList()->AddLine(ImVec2(x, y + cornerBoxHeight), ImVec2(x, y), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), thickness * 2);

		ImGui::GetForegroundDrawList()->AddLine(ImVec2((x + w) - cornerBoxLength, y), ImVec2(x + w, y), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), thickness * 2);
		ImGui::GetForegroundDrawList()->AddLine(ImVec2((x + w), y + cornerBoxHeight), ImVec2(x + w, y), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), thickness * 2);

		
		ImGui::GetForegroundDrawList()->AddLine(ImVec2(x + cornerBoxLength, (y + h)), ImVec2(x, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), thickness * 2);
		ImGui::GetForegroundDrawList()->AddLine(ImVec2(x, (y + h) - cornerBoxHeight), ImVec2(x, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), thickness * 2);

		
		ImGui::GetForegroundDrawList()->AddLine(ImVec2((x + w) - cornerBoxLength, (y + h)), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), thickness * 2);
		ImGui::GetForegroundDrawList()->AddLine(ImVec2((x + w), (y + h) - cornerBoxHeight), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), thickness * 2);
	}

	ImGui::GetForegroundDrawList()->AddLine(ImVec2(x + cornerBoxLength, y), ImVec2(x, y), ImGui::GetColorU32(color), thickness);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(x, y + cornerBoxHeight), ImVec2(x, y), ImGui::GetColorU32(color), thickness);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2((x + w) - cornerBoxLength, y), ImVec2(x + w, y), ImGui::GetColorU32(color), thickness);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2((x + w), y + cornerBoxHeight), ImVec2(x + w, y), ImGui::GetColorU32(color), thickness);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(x + cornerBoxLength, (y + h)), ImVec2(x, y + h), ImGui::GetColorU32(color), thickness);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(x, (y + h) - cornerBoxHeight), ImVec2(x, y + h), ImGui::GetColorU32(color), thickness);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2((x + w) - cornerBoxLength, (y + h)), ImVec2(x + w, y + h), ImGui::GetColorU32(color), thickness);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2((x + w), (y + h) - cornerBoxHeight), ImVec2(x + w, y + h), ImGui::GetColorU32(color), thickness);
}

void draw_cornered_box(int X, int Y, int W, int H, const ImU32& color, int thickness, int borderPx, bool outlined) {
	float lineW = (W / 3);
	float lineH = (H / 3);
	DrawFilledRect(X + borderPx, Y, W, borderPx, ImColor(10, 10, 10, 180));

	if (outlined) {
		ImGui::GetForegroundDrawList()->AddLine(ImVec2(X, Y), ImVec2(X, Y + lineH), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), 3);
		ImGui::GetForegroundDrawList()->AddLine(ImVec2(X, Y), ImVec2(X + lineW, Y), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), 3);
		ImGui::GetForegroundDrawList()->AddLine(ImVec2(X + W - lineW, Y), ImVec2(X + W, Y), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), 3);
		ImGui::GetForegroundDrawList()->AddLine(ImVec2(X + W, Y), ImVec2(X + W, Y + lineH), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), 3);
		ImGui::GetForegroundDrawList()->AddLine(ImVec2(X, Y + H - lineH), ImVec2(X, Y + H), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), 3);
		ImGui::GetForegroundDrawList()->AddLine(ImVec2(X, Y + H), ImVec2(X + lineW, Y + H), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), 3);
		ImGui::GetForegroundDrawList()->AddLine(ImVec2(X + W - lineW, Y + H), ImVec2(X + W, Y + H), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), 3);
		ImGui::GetForegroundDrawList()->AddLine(ImVec2(X + W, Y + H - lineH), ImVec2(X + W, Y + H), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), 3);
	}

	ImGui::GetForegroundDrawList()->AddLine(ImVec2(X, Y), ImVec2(X, Y + lineH), ImGui::GetColorU32(color), thickness);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(X, Y), ImVec2(X + lineW, Y), ImGui::GetColorU32(color), thickness);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(X + W - lineW, Y), ImVec2(X + W, Y), ImGui::GetColorU32(color), thickness);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(X + W, Y), ImVec2(X + W, Y + lineH), ImGui::GetColorU32(color), thickness);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(X, Y + H - lineH), ImVec2(X, Y + H), ImGui::GetColorU32(color), thickness);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(X, Y + H), ImVec2(X + lineW, Y + H), ImGui::GetColorU32(color), thickness);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(X + W - lineW, Y + H), ImVec2(X + W, Y + H), ImGui::GetColorU32(color), thickness);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(X + W, Y + H - lineH), ImVec2(X + W, Y + H), ImGui::GetColorU32(color), thickness);
}

void DrawNewTextCustom2(int x, int y, int R, int G, int B, int A, const char* str)
{
	ImGui::PushFont(GameFont);
	std::string utf_8_1 = std::string(str);
	std::string utf_8_2 = string_To_UTF8(utf_8_1);
	ImGui::GetBackgroundDrawList()->AddText(GameFont, 18.0f, ImVec2(x, y), ImGui::ColorConvertFloat4ToU32(ImVec4(R / 255.0, G / 255.0, B / 255.0, A / 255.0)), utf_8_2.c_str());
}