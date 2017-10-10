using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Timers;

//This script manages the player's weapon
public class FireControl : MonoBehaviour {

    public float reloadTime; //Time between shots in ms
    public GameObject bulletPrefab; //Bullet object

    Timer reloadTimer;
    bool readyFire;

	// Use this for initialization
	void Start () {
        readyFire = true;
        reloadTimer = new Timer(reloadTime);
        reloadTimer.Elapsed += Reloaded;
	}
	
	// Update is called once per frame
	void Update () {
        if (readyFire && Input.GetKey(KeyCode.Space))
        {
            readyFire = false;
            FireBullet();
            reloadTimer.Start();
        }
	}

    //Spawns a bullet
    void FireBullet()
    {
        GameObject bullet = GameObject.Instantiate(bulletPrefab);
        bullet.transform.position = this.transform.position;
    }

    void Reloaded(object source, ElapsedEventArgs e)
    {
        readyFire = true;
        reloadTimer.Stop();
    }
}
