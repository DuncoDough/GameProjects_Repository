using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class cannonCollider : MonoBehaviour
{
    [SerializeField] GameObject bullet1Prefab;
    [SerializeField] GameObject bullet2Prefab;
    [SerializeField] Transform cannonTip;
    Quaternion clampRotationLow, clampRotationHigh;

    Coroutine fire1coroutine, fire2coroutine;
    
    // Start is called before the first frame update
    void Start()
    {
        clampRotationLow = Quaternion.Euler(0,0,-70f);
        clampRotationHigh = Quaternion.Euler(0, 0,+70f);

    }

    // Update is called once per frame
    void Update()
    {
        PointAtMouse();

        if (Input.GetMouseButtonDown(0))
        {
            if (fire1coroutine == null) fire1coroutine = StartCoroutine(FireContinously(bullet1Prefab));
        }

        if (Input.GetMouseButtonDown(1))
        {
            if (fire2coroutine == null) fire2coroutine = StartCoroutine(FireContinously(bullet2Prefab));
        }

        if (Input.GetMouseButtonUp(0)) 
        { 
            StopCoroutine(fire1coroutine); 
            fire1coroutine = null; 
        }
        if (Input.GetMouseButtonUp(1)) 
        { 
            StopCoroutine(fire2coroutine); 
            fire2coroutine = null; 
        }

    }

    IEnumerator FireContinously(GameObject bullet1Prefab)
    {
        while (true)
        {
            Instantiate(bullet1Prefab, cannonTip.position, cannonTip.rotation);
            yield return new WaitForSeconds(1);
        }

    }


    private void PointAtMouse()
    {
       
        Vector3 relativePos = this.transform.position - GameData.MousePos;
        Quaternion newrotation = Quaternion.LookRotation(relativePos, Vector3.forward);
        newrotation.x = 0;
        newrotation.y = 0;
        newrotation.z = Mathf.Clamp(newrotation.z, clampRotationLow.z, clampRotationHigh.z);
        newrotation.w = Mathf.Clamp(newrotation.w, clampRotationLow.w, clampRotationHigh.w);
        this.transform.rotation = Quaternion.Slerp(this.transform.rotation, newrotation, Time.deltaTime * 3) ;
    }

    
}
