using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Timers;

//This script controls the behavior of bullets
public class BulletBehavior : MonoBehaviour {

    public float speed; //Speed at which the bullet travels
    public float range; //Distance bullet travels before despawning
    GameObject scorekeeper;

    // Use this for initialization
    void Start () {
        scorekeeper = GameObject.Find("Scorekeeper");
        Destroy(gameObject, range / speed); //Destroys the bullet after a delay based on range and speed
	}
	
	// Update is called once per frame
	void Update () {
        this.transform.Translate(new Vector3(0.0f, 0.0f, speed * Time.deltaTime));
    }

    //Called when bullet collides with something
    private void OnCollisionEnter(Collision collision)
    {
        //If the bullet collided with an object marked with target destroy them both
        if (collision.gameObject.CompareTag("Target"))
        {
            scorekeeper.GetComponent<Scorekeeper>().ScorePoint();
            Destroy(collision.gameObject);
            Destroy(gameObject);
        }
    }
}
