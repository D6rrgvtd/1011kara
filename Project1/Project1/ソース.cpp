
#include <Windows.h>

BOOL g_bShowEnd = FALSE;
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_DESTROY:  //×ボタン
		PostQuitMessage(0);
		return 0;
	case WM_PAINT: // ウィンドウの再描画が必要な時
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);

		// ENDの文字表示が有効な場合のみ描画する
		if (g_bShowEnd) {
			// ウィンドウの中心付近に文字を描画する設定
			RECT rect;
			GetClientRect(hwnd, &rect); // クライアント領域のサイズを取得

			// 文字色を赤に設定（任意）
			SetTextColor(hdc, RGB(250, 0, 0));
			// 背景モードを透過に設定 (文字の背景が透けるように)
			SetBkMode(hdc, TRANSPARENT);

			// "END"の文字を描画
			DrawText(
				hdc,
				TEXT("END"),
				-1, // 文字列の長さ（-1で自動計算）
				&rect,
				DT_CENTER | DT_VCENTER | DT_SINGLELINE // 領域内で中央揃え
			);
		}

		EndPaint(hwnd, &ps);
	}
	return 0;
	case WM_KEYDOWN: // キーが押されたとき
		// キー入力の処理をここに書く
		switch (wParam)
		{
		case VK_SPACE:
			g_bShowEnd = !g_bShowEnd;

			InvalidateRect(hwnd, NULL, TRUE);
			break;
		}
		return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
int WINAPI WinMain(
	HINSTANCE hInstance,  //アプリケーションの識別番号
	HINSTANCE hPreVInstance, //基本的に使わなくていい
	LPSTR IpCmdLine, //コマンドライン引数　(起動時のオプション)
	int nCmdShow  //ウィンドウの表示方法
)
{
	//1.ウィンドウクラス登録
	WNDCLASS wc{};
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = "GameWindow";
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);

	RegisterClass(&wc);

	HWND hwnd = CreateWindow(
		"GameWindow",        // ウィンドウクラス名
		"My Game",           // ウィンドウタイトル
		WS_OVERLAPPEDWINDOW,  // ウィンドウスタイル
		CW_USEDEFAULT, CW_USEDEFAULT,  // 位置（自動）
		800, 300,            // サイズ（幅、高さ）
		NULL, NULL,          // 親ウィンドウ、メニュー
		hInstance,           // インスタンス
		NULL                 // 追加データ
	);

	ShowWindow(hwnd, nCmdShow);  // ウィンドウを表示
	ShowWindow(hwnd, nCmdShow); //ウィンドウを表示
	//3.メッセージループ
	MSG msg{};
	while (GetMessage(&msg,NULL,0,0))
	{
		TranslateMessage(&msg);//キーボードメッセージを使いやすい形に変換
		DispatchMessage(&msg);//適切なウィンドウプロシージャに送信

	}


	return 0;
}