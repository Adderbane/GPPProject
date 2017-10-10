using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class Scorekeeper : MonoBehaviour {

    public Text scoreText;

    int score = 0;
    bool scoreChanged = false;

	// Use this for initialization
	void Start () {
		
	}
	
	// Update is called once per frame
	void Update () {
        if (scoreChanged)
        {
            scoreText.text = "Score: " + score;
            scoreChanged = false;
        }
	}

    //Function to call to increment score
    public void ScorePoint()
    {
        score++;
        scoreChanged = true;
    }
}
