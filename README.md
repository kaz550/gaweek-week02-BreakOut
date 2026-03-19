# Game A Week - Week2 Block Breaker (DxLib C++)

C++ × DxLib で制作した  
**Game A Week プロジェクト Week2「ブロック崩し」**です。

Week1で構築した基盤を流用し、  
**最小構成 → 手触り → 演出 → ステージ化 → 見た目強化**  
の流れで段階的に完成させています。

---

## 🎮 ゲーム概要

- ジャンル：2D ブロック崩し
- 解像度：1280×720
- フレームレート：60FPS目標（最低30FPS）
- 描画：DxLib プリミティブのみ（スプライト最小）

### 主な特徴

- 🎯 パドルでボールを反射
- 🧱 耐久ブロック（HP 1〜3）
- 💥 破壊演出（HitStop / Flash / Shake）
- ✨ 簡易Bloom（Glow）
- 🌀 ボール残像（トレイル）
- 🧠 面判定による反射（AABBベース）
- 🎮 ステージ制（3ステージ）
- 🏁 GameOver / StageClear / AllClear

---

## 🛠 使用技術

- 言語：C++
- ライブラリ：DxLib
- 描画：プリミティブ（DrawBox / DrawCircle）
- サウンド：DxLib Sound

---

## 🧩 アーキテクチャ

Week1で構築した基盤をそのまま使用：

- SceneManager / SceneContext
- Input（Action抽象化）
- AudioManager
- Camera2D（Shake）
- RenderUtil（Glow）
- HitStop / Flash
- AABB衝突

---

## 🔧 実装ハイライト

### ① 反射処理（Day4）

```cpp
bool Ball::ResolveVsAabb(const Aabb& target)
```

食い込み量から衝突面を判定

X or Y のみ反転

めり込み防止あり

👉 「なんとなく反射」から脱却

② 耐久ブロック（Day5）
```cpp
bool Block::Damage(int amount)
```

HP制（1〜3）

色変化

破壊時スコア加算

③ 演出（HitStop / Flash / Shake）
```cpp
m_hitStopTimer = 0.06f;
m_camera->AddShake(10.0f, 0.18f);
```

👉 触感を作る重要要素

④ Glow（疑似Bloom）
```cpp
RenderUtil::DrawGlowBox(...)
```

DxLibのみで実装

軽量・品質切替対応

⑤ ボール残像
```cpp
Vec2 m_trail[TRAIL_MAX];
```
リングバッファ

αブレンド描画

🎮 操作方法
操作	内容
← / →	パドル移動
決定キー	ボール発射
Back	Resultへ（デバッグ）
📸 スクリーンショット

（ここに画像貼るとGitHub映えます）

🚀 ビルド方法

DxLib を導入

Visual Studio でプロジェクトを開く

実行

📚 開発ログ（Zenn）

このプロジェクトは
Day1〜Day7の開発記録をZennで公開予定です

👉（ここにZennリンクをはる予定）

🎯 Game A Week について

毎週1本ゲームを作るプロジェクト：

Week1：回避ゲーム

Week2：ブロック崩し ← 本作

Week3：シューティング（予定）

✍️ 作者

HIDE
