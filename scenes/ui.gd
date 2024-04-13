extends CanvasLayer

func _process(delta):
	$Score.text = str(GameCore.score)
	$HighScore.text = str(GameCore.high_score)
