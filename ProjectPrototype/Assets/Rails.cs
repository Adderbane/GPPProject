using System.Collections;
using System.Collections.Generic;
using UnityEngine;

//This script is responsible for moving the player and camera along the Z direction, and any looping of scene
public class Rails : MonoBehaviour {

    public float speed = 1.0f;

	// Use this for initialization
	void Start () {
		
	}
	
	// Update is called once per frame
	void Update () {
        this.transform.Translate(new Vector3(0.0f, 0.0f, speed * Time.deltaTime));
    }
}
