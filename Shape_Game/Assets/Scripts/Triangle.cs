using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Triangle : Shape
{
    public float zigzagSpeed = 2.0f;

    // Start is called before the first frame update
    public override void Move()
    {
        base.Move();
        transform.Translate(Vector3.up * Mathf.Sin(Time.time * zigzagSpeed) * Time.deltaTime);
    }

    // Update is called once per frame
    void Update()
    {
        
    }
}
