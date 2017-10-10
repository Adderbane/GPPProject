using System.Collections;
using System.Collections.Generic;
using UnityEngine;

//This script is responsible for moving the player and camera along the Z direction, and any looping of scene
public class Rails : MonoBehaviour {

    public float speed; //Speed at which the player moves forward
    public float resetDist; //Distance before the player loops back to the beginning
    public float startPoint; //Z value the player starts at

	// Use this for initialization
	void Start () {
        this.transform.Translate(new Vector3(0.0f, 0.0f, startPoint));
	}
	
	// Update is called once per frame
	void Update () {
        this.transform.Translate(new Vector3(0.0f, 0.0f, speed * Time.deltaTime));
        if (this.transform.position.z >= resetDist)
        {
            this.transform.Translate(new Vector3(0.0f, 0.0f, -(resetDist - startPoint)));
        }
    }
}
