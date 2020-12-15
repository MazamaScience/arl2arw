# Let's unpack ARL! 

## Introduction


NOAA deals with data - lots of data. As such, NOAA created their own data-packing method to drastically reduce the necessary storage requirements. 


## Purpose



## Data Description


#### Network Common Data Format

#### Air Resource Laboratory Packed Data Format 

##### Data packing

ARL packed data files consist of numerous fixed-length records corresponding to each metoerological variable. The records are decribed at surface and upper-air levels and arranged in a time-series accordingly. Each record is compressed using a 50-byte ASCII header containing the date, time, variable, and data-packing constants. 

The meterological data grid points is packed with a compression algorithm utilizing the packing constant to reduce the data to single-byte grid points where each byte is a representation of the difference in value between itself and previous point. 

The algorithm is illustrated as follows: Assume that a grid of data is described with dimenisons `i,j`. The packed value is given by 

`P_i,j = (R_i,j - R_i-1, j) * (2*exp(7-N))` [1]

Where `N` is the scaling component such that `N = ln(dR_max / ln(2))`, where `dR_max` is the maximum difference between any point on the grid and its previous. The "packed" values are represented as single-byte unsigned integers where values range from 0 to 254 such that 0 to 126 represent -127 to -1, 127 represents 0, and 128 to 254 represent +1 to +127. Furthermore each packed integer value is stored as charcter data represented by the corresponding integer. Note that the correspondance between chracters and their integer representation is not necessarily identical for different Fortran compiler implementations[9]. More information about ARL can be found in the resources[1]. 

## Data Unpacking

Unpacking ARL data can be done by using the header to decode the index records using an unpacking algorithm described as the inverse packing algorithm: 

`U_i,j = ichar(P_i,j) - 127.0 / (2*exp(7-N) + P_1,1)` [5]

Where `ichar(P_i,j)` is the integer representation of the packed character data, `N` is the scaling component described above, and `P_1,1` is the first grid array element of the corresponding variable record. More infortmation about decoding ARL data files can be found in the resources [5].


#### Projection Mapping

## Resources 

- [1] https://www.ready.noaa.gov/archives.php

- https://smoke.airfire.org/bluesky-daily/output/temp_robert/4km/2020112500/

- [2] https://www.arl.noaa.gov/hysplit/cmapf-mapping-routines/

- [3] https://www.arl.noaa.gov/hysplit/hysplit-frequently-asked-questions-faqs/faq-5/

- [4] https://www.arl.noaa.gov/hysplit/cmapf-mapping-routines/using-the-emapf-functions-fortran/

- [5] https://ready.arl.noaa.gov/hysplitusersguide/S141.htm

- [6] https://www.arl.noaa.gov/hysplit/cmapf-mapping-routines/cmapf-faq/

- [7] https://opensky.ucar.edu/islandora/object/opensky:2898

- [8] http://amps-backup.ucar.edu/information/configuration/wrf_grid_structure.html

- [9] http://fortranwiki.org/fortran/show/ichar