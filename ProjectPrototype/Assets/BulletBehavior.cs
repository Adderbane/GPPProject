using System.Collections;
using System.Collections.Generic;
using UnityEngine;

//This script controls the behavior of bullets
public class BulletBehavior : MonoBehaviour {

    public float speed; //Speed at which the bullet travels

    // Use this for initialization
    void Start () {
		
	}
	
	// Update is called once per frame
	void Update () {
        this.transform.Translate(new Vector3(0.0f, 0.0f, speed * Time.deltaTime));
    }
}
