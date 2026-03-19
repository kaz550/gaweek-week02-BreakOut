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
