# MPI_pro
Pro using MPI

dimension of matrix is 100 * 100:
    
    normal: 0.014s
    
    4-process:0.0039s
    
    16-process:0.00179s
    
    25-process: 0.0019s

dimension of matrix is 256 * 256:

    normal method:0.258287 
    
    4-process cannon:0.058600
    
    16-process cannon:0.018291
    

dimension of matrix is 400 * 400:

    normal method:0.933232 
    
    4-process cannon:0.223899
    
    16-process cannon: 0.06411
    
    25-process cannon:0.045077 

dimension of matrix is 900 * 900:

    normal: 10.9s

    81-process:0.521s
    
dimension of matrix is 1000 * 1000:

    normal: 13.73s
    
    16-process: 1.03s
    
    100-process: 0.69s

It has strong scalility.
