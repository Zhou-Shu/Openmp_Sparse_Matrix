# openmp_Big_Sparse_Matrix

Using parallel algorithms and cross-linked lists to deal with large sparse matrices


## Dense Matrix File Format

datatype
rowcol
matrix

For an example of a float matrix float1.in
```
float 
44 
1.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 1.0 0.5 0.0 0.0 0.0 0.0 0.75 
```

## Command Line Arguments Glossary

1. Several operators, determines what matrix operation will be performed
(have been discussed previously)

  - --sc - Scalar multiplication
  
  - --tr - Trace
  
  - --ad - Addition
  
  - --ts - Transpose
  
  - --mm - Matrix multiplication
  
2. -f %s (%s) depending on the operation requested, one or more matrix
files will need to be passed
  - Example: ./mysolution.exe --sc -f matrix1.in
  - Example: ./mysolution.exe --mm -f matrix1.in matrix2.in
3. -l Log. If present, results will be logged to file

