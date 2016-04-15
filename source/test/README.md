## Bench Mark Test

### Red Black Tree  
     Red black tree in cdsl is tested compared to C++ std map for the case of non-primitive typed value
     for the fairness of the test, separate processor is used because the test is performed in same process in sequential
     manner, the heap fragmentation can cause performance degradation. so test performed later always handicapped than earlier.
     by separating the test process, this kind of impact can be minimized. although Red black tree of cdsl has no dependency to
     dynamic allocation of memory, it's forced to use dynamic allocation for the fairness.  

#### Test Condition & Measuring
     Single Test is consist of insertion / lookup / deletion operation with 2,000,000 items and the operation time measured in second.
     and take an average of each measured time after repeat the 10 times of test.
     
#### Test Reslut    
      Red Black Tree (cdsl) 
      - insertion : 0.289357s
      - lookup    : 0.220192s
      - deletion  : 0.112507s
      
      Map (C++ std)
      - insertion : 0.534634s
      - lookup    : 0.257456s
      - deletion  : 0.209501s    
     
        
     