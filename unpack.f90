module unpack

  implicit none

  real, allocatable :: rdata(:, :)
  character(1), allocatable :: cdata(:)

  character(80) :: arl
  character(50) :: header_label 
  character(3072) :: header

  integer :: year, month, day, hour
  integer :: ifc
  character(4) :: var_desc

  character(4) :: model
  integer :: grid_num, z_coord
  real :: pole_lat, pole_lon, ref_lat
  real :: ref_lon, grid_size, orient
  real :: tan_lat, sync_x, sync_y
  real :: sync_lat, sync_lon, reserved
  integer :: nx, ny, nz

  integer :: nxy
  integer :: rec_len

  integer :: rstat, cstat, iostat

  integer :: i, k
  integer :: nexp 
  real :: var1, prec

  interface
    ! unpacked character data is resolved to real_data 
    subroutine decode(cdata, rdata, nx, ny, nexp, var1)
        character(1), intent(in) :: cdata(:)
        real, intent(out) :: rdata(:, :)
        integer, intent(in) :: nx, ny, nexp
        real, intent(in) :: var1
    end subroutine
  end interface

  write(*,*) 'Enter ARL file name:'
  read(*, '(a)') arl
  arl = adjustl(arl)

  open(10, file = arl, recl = 150, access = 'direct', form = 'unformatted')
  read(10, rec = 1) header_label, header(1:108)
  read(header_label,'(5i2,4x,a4)') year, month, day, hour, ifc, var_desc
  !! Debug
  ! write(*, '(a, 4i2)') 'YYMMDDHH: ', year, month, day, hour
  read(header(1:108), '(a4, 1i3, 1i2,  12f7.0, 3i3)')         & 
       model, grid_num, z_coord,                              &
       pole_lat, pole_lon, ref_lat,                           &
       ref_lon, grid_size, orient,                            &
       tan_lat, sync_xp, sync_yp,                             &
       sync_lat, sync_lon, reserved,                          &
       nx, ny, nz  
  close(10)

  nxy = nx * ny
  rec_len = nxy + 50
  
  allocate(real_data(nx,ny), stat = rstat)
  allocate(char_data(nxy), stat = cstat)

  open(10, file = arl, recl = rec_len, acces = 'direct', form = 'unformatted')

  i = 1
  do 
    read(10, rec = i, iostat = iostat) label, (cdata(k), k = 1, nxy)
    if ( iostat > 0 ) then 
      exit
    else 
      read(label, '(6i2, 2x, a4, 1i4, 2f7.0)') & 
      year, month, day, hour, grid_num, level, & 
      var_desc, nexp, prec, var1
      ! unpack data from header 
      if(var_desc /= 'INDX')  then 
        ! call decode(cdata, rdata, nx, ny, nexp, var1)   
      end if
    end if 
    i = i + 1
  end do

end module

! subroutine decode(cdata, rdata, nx, ny, nexp, var1) 

!     character(1), intent(in) :: cdata(:)    
!     real, intent(out) :: rdata(:, :)
!     integer, intent(in) :: nx, ny, nexp
!     real, intent(in) :: var1
!     real :: scale
!     real :: vold
!     integer :: indx
!     integer :: i, j

!     scale = 2.0 ** (7 - nexp)
!     vold = var1 
!     indx = 0

!     do j = 1, ny
!         do i = 1, nx 
!             indx = indx + 1
!             rdata(i,j) = (ichar(cdata(indx)) - 127.0) / scale + vold
!             vold = rdata(i,j)
!         end do
!         vold = rdata(1,j)
!     end do

! end subroutine