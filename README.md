## 「VEIL」

3Dアクションゲームとフレームワーク
* 個人制作
* •開発言語：C++
* ライブラリ：DirectX11, DirectXTex, Assimp, ImGui
* ツール：Visual Studio, Git, Blender, IBLBaker
* 制作時間：約4ヶ月

## プレイ動画
https://github.com/user-attachments/assets/a1f8448a-c97f-4d3c-8f4c-982f3155ba37

##　ファイル構成

```
├─resource
│  ├─shader ---- シェーダー
├─src
│  ├─component　---- コンポーネント（描画・当たり判定など機能で利用）
│  ├─config
│  ├─editor　---- AssimpによるUI
│  ├─math
│  ├─object　----　オブジェクトとゲームの実装
│  ├─physics ---- 物理システム（簡易当たり判定）
│  ├─platform
│  ├─render ---- 描画システム
│  │  ├─config ---- ゲーム側でメッシュなどの描画できるものを表現するためのデータ
│  │  ├─particle ---- GPUパーティクルの管理
│  │  ├─pass ---- レンダリングパスで再利用可能な処理ステップ
│  │  ├─path ---- レンダリングパス、カメラ描画に必要な処理手順の管理
│  │  ├─resource ---- 常用GPUリソースとAPIのラッパー
│  │  └─util ---- レンダリングパス
│  ├─scene
│  └─util
```

## 主要な機能とファイル

描画システム `src/render/render_system.h`

カメラシステム `src/render/render_camera.h`

> 複数カメラや複数レンダリングパスに対応した構造を実装しました。
> カメラごとに異なる描画処理や描画対象を設定できるようにしており、環境マップへの描画を行う特殊なカメラにも対応しています。この仕組みにより、例えばシャドウマップ用のカメラ（チーム制作で使用）や平面反射用カメラなど、用途に応じたカメラを拡張しやすい設計にしています。

レンダリングパス `src/render/render_path.h`

> 1回のカメラ描画に必要な処理手順を管理する役割を持ちます。各処理ステップ`src/render/pass/`は再利用できる構造になっており、例えばG Bufferの生成`src/render/pass/geometry/subpass_geometry_default.h`や、クリーンスペースリフレクション(SSR)`src/render/pass/postprocess/pass_ssr.h`などを組み合わせて利用できるようにしました。



## セールスポイント

### シェーダーによる質感表現
* 物理ベースレンダリング
* ポストプロセスエフェクト
    * ブルーム
    * スクリーンスペースリフレクション
* 特殊なシェーダー
    * セルシェーダー
    * 自動的に解像度に対応できるLCDパネルシェーダー

<p>
<img alt="feature_ibl" src="doc/images/feature_ibl.gif" height="120">
<img alt="feature_lcd" src="doc/images/feature_lcd.gif" height="120">
</p>

### 複数のカメラによる演出
自作フレームワークでレンダーテキスチャーを普通のテキスチャーと同じように設定できます
<p>
<img alt="feature_camera" src="doc/images/feature_camera.png" height="120">
</p>

### 多様な反射表現
* 平面反射
* スクリーンスペースリフレクション
* 環境マッピング

<p>
<img alt="feature_reflect" src="doc/images/feature_plane_reflect.gif" height="100">
<img alt="feature_ssr" src="doc/images/feature_ssr.png" height="100">
<img alt="feature_reflect" src="doc/images/feature_env_map.png" height="100">
</p>


### 効率的な実装
* デファードレンダリング

* GPU インスタンシング

* コンピュートシェーダー

* メモリ効率の改善


### 汎用的なフレームワーク

作成したフレームワークは、チーム制作[https://github.com/tkou2027/at28-dash](https://github.com/tkou2027/at28-dash)でも利用されています



## 参考資料

描画
* src/render/util/dx_trace.h、src/render/util/dx_trace.cpp  参考元：GitHub MKXJun/DirectX11-With-Windows-SDK
* src/render/util/gpu_timer.h、src/render/util/gpu_timer.cpp  参考元：GitHub MKXJun/DirectX11-With-Windows-SDK

シェーダー
* resource/shader/feature/shading_pbr.hlsl 参考元：LearnOpenGL
* resource/shader/pixel_blur_dual_up.hlsl、resource/shader/pixel_blur_dual_down.hlsl 参考元：LearnOpenGL
* resource/shader/pixel_forward_screen.hlsl 参考元：GitHub CJT-Jackton/URP-LCD-Dispaly-Example （Mipmap Level計算部分）
* resource/shader/feature/shading.hlhl 参考元：GitHub ashyukiha/GenshinCharacterShaderZhihuVer（RampShadow部分）

その他
* src/math/aabb.h、src/math/aabb.cpp、src/math/interval.h、src/math/interval.cpp 参考元：Ray Tracing in One Weekend
* src/util/debug_ostream.h、src/util/debug_ostream.cpp 授業での配布資料
* src/platform/keyboard.h、src/platform/keyboard.cpp 授業での配布資料
* src/platform/sound.h、src/platform/sound.cpp 授業での配布資料