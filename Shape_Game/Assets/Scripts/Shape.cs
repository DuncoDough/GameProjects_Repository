using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Shape : MonoBehaviour
{
    public float moveSpeed = 2.0f;

    // Start is called before the first frame update
    public virtual void Move()
    {
        
        transform.Translate(Vector3.right * moveSpeed * Time.deltaTime);

    }

    // Update is called once per frame
    void Update()
    {
    
        Move();
    }
}
