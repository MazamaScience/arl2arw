program arl2arw
!------------------------------------------------------------------------------
!
! Program to convert the packed ARL meteorological file format to WRF netCDF
! More information for ARL packed metorological format: 
! - https://ready.arl.noaa.gov/hysplitusersgudaye/S141.htm
! - https://www.ready.noaa.gov/archives.php
! - ftp://arlftp.arlhq.noaa.gov/pub/archives/utility/chk_data.f
!
!------------------------------------------------------------------------------
    implicit none

    real, allocatable :: real_data(:, :) ! 2d unpacked data 
    character(1), allocatable :: char_data(:) ! 1d packed data

    character(80) :: file ! prompt file name
    character(50) :: label ! header label 
    character(3072) :: header ! file header

    integer :: year, month ! year and month
    integer :: day, hour ! day and hour
    character(4) :: var_desc ! descriptor of variable being written
    integer :: ifc ! No clue 
    integer :: level ! level of z 
    integer :: k, n ! place holders
    integer :: nexp ! integer scaling exp of packaed data array
    real :: var1 ! real value of array at (1, 1)
    real :: prec ! real precision of packed data array

    character(4) :: model ! data model character
    integer :: grid_num ! grid number
    integer :: z_coord ! z coordinate
    real :: pole_lat, pole_lon, ref_lat, ref_lon, grid_size, orient, & 
            tan_lat, sync_xp, sync_yp, sync_lat, sync_lon, reserved
    integer :: nx, ny, nz, nxy ! number of x, y points, z levels
    integer :: recl ! record length
    
    integer :: alstat ! /= 0 if not enough memory 
    integer :: iostat ! > 0 if not enough memory 

    ! unpacked character data is resolved to real_data 
    interface
        subroutine unpack(char_data, real_data, nx, ny, nexp, var1)
            character(1), intent(in) :: char_data(:)
            real, intent(out) :: real_data(:, :)
            integer, intent(in) :: nx, ny, nexp
            real, intent(in) :: var1
        end subroutine
    end interface
    
    ! get file name
    write(*,*) 'Enter a file name:'
    read(*, '(a)') file
    file = adjustl(file)

    ! open the file and decode the 50-byte header plus the fixed portion
    open(10, file = file, recl = 158, access = 'direct', form = 'unformatted')

    ! decode the "standard" portion of the index record
    read(10, rec = 1) label, header(1:108)
    read(label,'(5i2,4x,a4)') year, month, day, hour, ifc, var_desc
    write(*, '(a, 4i2)') 'YYMMDDHH: ', year, month, day, hour
    
    ! decode extended portion of header
    ! field descriptions: https://ready.arl.noaa.gov/hysplitusersguide/S141.htm
    read(header(1:108), '(a4, 1i3, 1i2,  12f7.0, 3i3)')         & 
         model, grid_num, z_coord,                              &
         pole_lat, pole_lon, ref_lat,                           &
         ref_lon, grid_size, orient,                            &
         tan_lat, sync_xp, sync_yp,                             &
         sync_lat, sync_lon, reserved,                          &
         nx, ny, nz  
    
    ! close the header read
    close(10)

    nxy = nx * ny ! record size
    recl = nxy + 50 ! record size + header length

    ! properly re-open the file with the record length
    open(10, file = file, recl = recl, access = 'direct', form = 'unformatted')

    print *, 'Model: ', model, new_line('a'),                           & 
             'Grid Number: ', grid_num, new_line('a'),                  & 
             'Z-Coordinate: ', z_coord, new_line('a'),                  & 
             'Pole (lat,lon): ', pole_lat, pole_lon, new_line('a'),     & 
             'Reference (lat,lon): ', ref_lat, ref_lon, new_line('a'),  & 
             'Orientation: ', orient, new_line('a'),                    & 
             'Tangent Latitude: ', tan_lat, new_line('a'),              & 
             'Synch (x,y): ', sync_xp, sync_yp, new_line('a'),          & 
             'Synch (lat,lon): ', sync_lat, sync_lon, new_line('a'),    & 
             'Reserved: ', reserved, new_line('a'),                     &
             'Number of X points: ', nx, new_line('a'),                 & 
             'Number of Y points: ', ny, new_line('a'),                 & 
             'Number of Levels: ', nz, new_line('a')

    ! allocate the character and real arrarys of proper dimensions
    allocate(real_data(nx,ny), stat = alstat)
    allocate(char_data(nxy), stat = alstat)

    ! read the file and upack its content     
    n = 1 
    do
        ! read each header label
        read(10, rec = n, iostat = iostat) label, (char_data(k), k = 1, nxy) 
        if (iostat > 0) then ! >0 is error is thrown / end of records(?)
            exit 
        else
            read(label, '(6i2, 2x, a4, 1i4, 2f7.0)') & 
            year, month, day, hour, grid_num, level, & 
            var_desc, nexp, prec, var1
            ! unpack data from header 
            if(var_desc /= 'INDX')  then 
                call unpack(char_data, real_data, nx, ny, nexp, var1)   
            end if
            ! preview a bit first row of each data
            print *, level, var_desc, var1, real_data(2:3, 1), '...'
        end if
        n = n + 1
        ! TODO: implement netcdf writeout
    end do

end program arl2arw

subroutine unpack(char_data, real_data, nx, ny, nexp, var1)
    
    character(1), intent(in) :: char_data(:)    
    real, intent(out) :: real_data(:, :)
    integer, intent(in) :: nx, ny, nexp
    real, intent(in) :: var1

    scale = 2.0 ** (7 - nexp)
    vold = var1 
    indx = 0

    do j = 1, ny
        do i = 1, nx 
            indx = indx + 1
            real_data(i,j) = (ichar(char_data(indx)) - 127.0) / scale + vold
            vold = real_data(i,j)
        end do
        vold = real_data(1,j)
    end do

end subroutine

