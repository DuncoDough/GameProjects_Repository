using JetBrains.Annotations;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Circle : Shape
{
    public float rotationSpeed = 50.0f;
    // Start is called before the first frame update
    public override void Move()
        {
        base.Move();
        transform.Rotate(Vector3.forward * rotationSpeed * Time.deltaTime);
    }

    // Update is called once per frame
    void Update()
    {
        
    }
}
