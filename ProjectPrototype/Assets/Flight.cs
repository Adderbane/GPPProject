using System.Collections;
using System.Collections.Generic;
using UnityEngine;

//This Script is responsible for moving the player around in the XY directions.
public class Flight : MonoBehaviour {

    public float slideRate; //The rate at which the player's ship moves in the XY Plane

	// Use this for initialization
	void Start () {

    }
	
	// Update is called once per frame
	void Update () {

        bool left = Input.GetKey(KeyCode.A);
        bool right = Input.GetKey(KeyCode.D);
        bool up = Input.GetKey(KeyCode.W);
        bool down = Input.GetKey(KeyCode.S);

        Vector3 moveDir = Vector3.zero;

        if ((left && this.transform.position.x > -4.0f))
        {
            moveDir.x -= 1.0f;
        }
        if ((right && this.transform.position.x < 4.0f))
        {
            moveDir.x += 1.0f;
        }
        if ((down && this.transform.position.y > -3.0f))
        {
            moveDir.y -= 1.0f;
        }
        if ((up && this.transform.position.y < 3.0f))
        {
            moveDir.y += 1.0f;
        }

        moveDir = moveDir.normalized * slideRate * Time.deltaTime;

        this.transform.Translate(moveDir);

	}
}
