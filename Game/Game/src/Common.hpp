
# pragma once
# include <Siv3D.hpp> // OpenSiv3D v0.4.1
constexpr auto DEPTH = 50; //探索の最大の深さ
// シーンの名前
enum class State
{
	Title,

	Game
};

// ゲームデータ
struct GameData
{
	// ハイスコア
	int32 highScore = 0;
  
  int32 tmpTime = 0;
};


// シーン管理クラス
using MyApp = SceneManager<State, GameData>;
