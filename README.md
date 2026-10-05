# Deep Sea Diver 🤿

以 C++ 與 Allegro 5 從零實作的 2D 橫向捲軸深海探險遊戲，約 3,000 行程式、10 個類別模組。課程規定只能使用 C++ 與 Allegro、必須採用物件導向設計且不得使用現成模板，因此遊戲迴圈、狀態管理、碰撞、鏡頭與音效皆為自行設計。

## 遊戲概念

玩家扮演潛水員，在隨機生成的海底地形中以魚叉捕魚、打撈寶箱換取金幣，並在水面商店升級裝備。氧氣持續下降，必須在探索深度與返回水面之間取捨。地圖深處的傳送門通往第二關「黑暗海域」，有發光的鮟鱇魚、水母與更高價值的寶藏。

## 技術重點

| 項目 | 說明 |
| --- | --- |
| 物件導向架構 | Game、Diver、Fish、Harpoon、GameMap、Renderer、Shop、Treasure、LeaderboardSystem 等類別；邏輯更新與繪圖分離 |
| 有限狀態機 | 遊戲流程 10 種狀態（選單、設定、遊戲中、黑暗海域、商店、排行榜…）；魚叉另有 Ready → Aiming → Firing → Retracting / Struggling 狀態機 |
| 程序化地圖 | 3000×1500 海底地形以隨機斜率生成，隨機配置岩柱、珊瑚、海草，搭配跟隨玩家的捲動鏡頭 |
| 敵人 AI | 鯊魚與鮟鱇魚依距離判斷追擊，咬到玩家後進入撤退計時；鯊魚需命中 3 次才能捕獲 |
| 遊戲機制 | 瞄準時慢動作（0.1×）、雷射瞄準線擺動；捕到大魚觸發連打 B 鍵角力小遊戲 |
| 經濟與存檔 | 商店升級蛙鞋、氧氣瓶、魚叉，價格隨等級遞增；排行榜以檔案讀寫並排序 |
| 碰撞與繪圖 | AABB 碰撞判定；精靈圖依方向縮放、旋轉、翻轉；場景切換背景音樂與音量調整 |

## 程式結構

```
main.cpp            進入點
Game.*              遊戲迴圈、狀態機、事件處理
Diver.* Fish.*      玩家與魚群（含 AI）
Harpoon.*           魚叉狀態機與角力小遊戲
Map.*               程序化地形生成與碰撞
Draw.*              繪圖（Renderer）
Shop.* Treasure.*   商店與寶藏
Leaderboard.*       排行榜檔案存取
GameConfig.h Unit.h 共用設定與基底類別
```

## 編譯與執行（Windows + MinGW）

```bash
g++ main.cpp Game.cpp Leaderboard.cpp Draw.cpp Map.cpp Shop.cpp Treasure.cpp Fish.cpp Harpoon.cpp Diver.cpp \
    -o my_game.exe -I allegro/include -L allegro/lib -lallegro_monolith
```

或在 VS Code 中執行預設建置工作（`.vscode/tasks.json`）。執行時需將 `allegro/bin` 中的 DLL 放在執行檔旁。

**操作**：方向鍵移動、按住 SPACE 瞄準並放開發射、捕到大魚時連打 B。

Demo 影片：`group47.mp4`
