# STADS Electrical Repository
This repository contains all the codes intended to be used on hardware, including VHDL/Verilog, Embedded C, C++ for HLS and bitstreams to program FPGAs.

## Organization of codes
1. Put code in the respective directories following the file nomenclature explained in [QA Guidelines] (https://docs.google.com/document/d/1U63RPCxErLuVvC3BmJ-6Fqa9EqF_KslVQfur7OiazCk/edit "QA Guidelines").
2. Appropriate Version Control Practices to be followed as mentioned in the QA Guidelines.
3. Any arguments to be fed into C/C++ codes to be given as arguments to main and run from the command line. The main should look as follows:
```
int main(int argc, char** argv)
{
    \\ insert main code here
}
```
4. Do not include any non-code/bitstream files (like .txt, .jpg, .png, etc) in the repository unless absolutely necessary.
