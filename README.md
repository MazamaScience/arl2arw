# arl2arw

#### This application is still under development

arl2arw is program used for converting the HYSPLIT compatible Air Resources Laboratory (ARL) Packed Data Format to WRF-_ish_ Network Common Data Format.

## Getting Started

The application is written with an icluded make file for simple compilation using GCC. arl2arw **requires** the NetCDF C libraries to be installed. 

#### Compile

```
make compile
```

#### Run 

```
./arl2arw YOUR_ARL_FILE YOUR_NC_FILE
```
