using System.Collections;
using System.Collections.Generic;
using UnityEngine;

//This script is responsible for setting up targets to shoot in each pass
public class TargetManager : MonoBehaviour {

    public Vector3 origin;
    public Vector3 size;
    public int targetCount;

    public GameObject target;

	// Use this for initialization
	void Start () {
        PopulateRegion();
	}
	
	// Update is called once per frame
	void Update () {
		
	}

    public void PopulateRegion()
    {
        for (int i = 0; i < targetCount; i++)
        {
            Vector3 spawnPos = new Vector3(Random.Range(origin.x - size.x / 2.0f, origin.x + size.x / 2.0f),
                                           Random.Range(origin.y - size.y / 2.0f, origin.y + size.y / 2.0f),
                                           Random.Range(origin.z - size.z / 2.0f, origin.z + size.z / 2.0f));

            GameObject.Instantiate(target, spawnPos, Quaternion.identity);
        }
    }
}
