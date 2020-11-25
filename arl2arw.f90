program arl2arw
    !implicit none

!------------------------------------------------------------------------------
!
! Program to convert the packed ARL format to WRF netCDF
!
!------------------------------------------------------------------------------

    real, allocatable :: rdata(:,:)
    character(1), allocatable :: char_pack(:) 

    character(80) :: file
    character(50) :: label
    character(256) :: header

    integer :: iy, im ! year and month
    integer :: id, ih ! day and hour
    character(4) :: kvar ! descriptor of variable being written
    integer :: ifc ! No clue 

    character(4) :: data_src ! data source (model)
    integer :: forecast_hr ! model forecast hour
    integer :: minutes ! model forcast minute
    real :: pole_lat, pole_lon, ref_lat, ref_lon, grid_size, orient, tan_lat, sync_xp, sync_yp, sync_lat, sync_lon, reserved
    integer :: nx, ny, nz ! number of x, y points, z levels
    integer :: vert_flag ! vertical coordinate system flag 
    integer :: lenh ! length in bytes of the header index record, excluding the first 50 bytes 
    integer :: recl ! record length

    interface
        subroutine unpack(char_pack, rdata, nx, ny, nexp, var1)
            character(1), intent(in) :: char_pack(:)
            real, intent(out) :: rdata(:, :)
            integer, intent(in) :: nx, ny, nexp
            real, intent(in) :: var1
        end subroutine
    end interface
    
    ! Get file name
    write(*,*) 'Enter a file name:'
    read(*, '(a)') file
    file = adjustl(file)

    ! open the file and decode the 50-byte header plus the fixed portion
    open(10, file = file, recl = 158, access = 'direct', form = 'unformatted')

    ! decode the standard portion of the index record
    read(10, rec = 1) label, header(1:108)
    read(label,'(5i2,4x,a4)') iy, im, id, ih, ifc, kvar
    
    ! decode extended portion of header
    read(header(1:108), '(a4, i3, i2, 12f7.0, 3i3, i2, i4)') & 
         data_src, forecast_hr, minutes,                    &
         pole_lat, pole_lon, ref_lat,                       &
         ref_lon, grid_size, orient,                        &
         tan_lat, sync_xp, sync_yp,                         &
         sync_lat, sync_lon, reserved,                      &
         nx, ny, nz,                                        &
         vert_flag, lenh
    
    ! close the header read
    close(10)

    
    nxy = nx * ny ! record size
    recl = nxy + 50 ! record length

    ! properly re-open the file
    open(10, file = file, recl = recl, access = 'direct', form = 'unformatted')

    write(*, '(a, a4)') 'Model : ', data_src
    write(*, '(a, i3)') 'Forcast hour : ', forecast_hr  
    write(* , '(a, i2)') 'Model minutes: ', minutes
    write(*, '(a, 1i5)') 'Grid width : ', nx
    write(*, '(a, 1i5)') 'Grid height : ', ny
    write(*, '(a, 1i5)') 'Grid count : ', nxy 
    write(*, '(a, 1i2)') 'Levels : ', nz
    write(*, '(a, 1i5)') 'Header record size : ', lenh

    allocate(rdata(nx,ny), stat = kret)
    allocate(char_pack(nxy), stat = kret)

    ! read the file and upack its content     
    do n = 1, nz

        read(10, rec = n) label, (char_pack(k), k = 1, nxy)
        read(label, '(6i2, 2x, a4, 1i4, 2f7.0)') & 
            iy, im, id, ih, ifr, kl, & 
            kvar, nexp, prec, var1
        write(*, '(a)') label
        if(kvar /= 'INDX') call unpack(char_pack, rdata, nx, ny, nexp, var1)

        ! TODO: implement netcdf writeout
        
    end do

end program arl2arw

subroutine unpack(char_pack, rdata, nx, ny, nexp, var1)
    
    character(1), intent(in) :: char_pack(:)    
    real, intent(out) :: rdata(:, :)
    integer, intent(in) :: nx, ny, nexp
    real, intent(in) :: var1

    scale = 2.0 ** (7 - nexp)
    vold = var1 
    indx = 0

    do j = 1, ny

        do i = 1, nx 

            indx = indx + 1
            rdata(i,j) = (ichar(char_pack(indx)) - 127.0) / scale + vold
            vold = rdata(i,j)
            if (i <= 2 .and. j <= 2) & 
                write(*, '(3i5, e12.4)') j, i, ichar(char_pack(indx)), rdata(i,j)
            if (i >= (nx-1) .and. j >= (ny-1)) & 
                write(*, '(3i5, e12.4)') j, i, ichar(char_pack(indx)), rdata(i,j)

        end do

        vold = rdata(1,j)

    end do

end subroutine

