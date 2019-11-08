
# pragma once
# include "Common.hpp"

// ゲームシーン
class Game : public MyApp::Scene
{
private:

	// ブロックのサイズ
	static constexpr Size blockSize = Size(40, 20);

	// ボールの速さ
	static constexpr double speed = 480.0;

	// ブロックの配列
	Array<Rect> m_blocks;

	// ボールの速度
	Vec2 m_ballVelocity = Vec2(0, -speed);

	// ボール
	Circle m_ball = Circle(400, 400, 8);

	// パドル
	Rect m_paddle = Rect(Arg::center(Cursor::Pos().x, 500), 60, 10);

	// スコア
	int32 m_score = 0;


	//追加
	/*
	typedef struct {
		int x;
		int y;
		int status;
		int turn;
	}Coordinate;
	*/
	/*
	Coordinate record[64] = { 0 }; //一手ごとに記録する
	int innerCell[10][10] = { 0 }; //石の場所を記憶する 領域外も含む
	int deepCell[100][10][10] = { 0 }; //ｎ手先を読む用
	int turn = 1; //手数を数える

	int stack[60] = { 0 };
	int sp = 0;
	*/
	/*
	int flip_stone(int, Coordinate, int);
	void push(int);
	int pop();
	int staticScore(int);
	*/

public:

	Game(const InitData& init);

	void update() override;

	void draw() const override;
};