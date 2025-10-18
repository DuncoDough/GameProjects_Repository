using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Missile : DefaultBullet
{
    // Start is called before the first frame update
    protected override void Start()
    {
        base.Start();
        rb.drag = 0.5f;
    }

    // Update is called once per frame
    void Update()
    {
        
    }
}
