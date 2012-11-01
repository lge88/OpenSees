# cd /home/GL/Develop/server/node-opensees-server/files/shared/projects/3HOP-no-wall;
# wipe all;

# --------------------------------------------------------------------------------------------------
# script by Richard Wood
# units are presribed as: kip, inch, second
# see accompanying Risa Figure for details
# using Newmark's integration method;
# alpha = 0.5;
# beta = 0.25;
 
# SET UP ----------------------------------------------------------------------------
set ROOT_DIR /home/GL/Develop/server/node-opensees-server/files/shared
set PROC_DIR $ROOT_DIR/proc
set TS_DIR $ROOT_DIR/ts
set OUT_DIR_PREFIX .

wipe analysis;
wipe;						# clearing opensees model
model basic -ndm 2 -ndf 3;				# 2 dimensions, 3 dof per node


#source input_parameters.txt
set dir $OUT_DIR_PREFIX/record1_2_iter_1_rl_1 ;
set scale 0.096813 ;
set eq_record $TS_DIR/record1_2.txt ;
set Npoints 3000 ;
set dt 0.01 ;

file mkdir $dir; 					# create data directory
puts $dir
puts "3 HOSP File Loaded - working..."

# define variables-------------------------------------------------------------
set damping 0.05;
set g 386.4;


# define GEOMETRY -------------------------------------------------------------


# Define ELEMENTS -------------------------------------------------------------
geomTransf Corotational 1;


#set dir config_a ;
#source building_files/nodal_a.txt ;
node	1	0.00	0.00	;
node	2	180.00	0.00	;
node	3	360.00	0.00	;
node	4	0.00	240.00	;
node	5	180.00	240.00	;
node	6	360.00	240.00	;
node	7	0.00	432.00	;
node	8	180.00	432.00	;
node	9	360.00	432.00	;
node	10	0.00	624.00	;
node	11	180.00	624.00	;
node	12	360.00	624.00	;


# Single point constraints -- Boundary Conditions
fix 1 1 1 1; 			# node DX DY RZ (node 1)
fix 2 1 1 1;			# fixing node 2 as well
fix 3 1 1 1;

#source building_files/material.txt ;
#######Define material model (steel02)#########################
# with isotropic strain hardening.
set steelID 201;				# material tag
set Fy	50.0;		# yield stress for A992 steel material
set Es	29000.0;	# elastic modulus for steel material
set b		0.02;				# post yield slope ratio
set R0	10;				# control the transition from elastic to plastic branches
set cR1	0.925;			# Recommended values:
set cR2	0.15;				# $R0=between 10 and 20, $cR1=0.925, $cR2=0.15
#- Optional ---------------------------------------------------------------------------------------------------#
set a1	0.0;				# isotropic hardening parameter, increase of compression yield envelope as
			# proportion of yield strength after a plastic strain of $a2*($Fy/$E)
set a2	1.0;				# isotropic hardening parameter (see explanation under $a1)
set a3	0.0;				# isotropic hardening parameter, increase of tension yield envelope as
			# proportion of yield strength after a plastic strain of $a4*($Fy/$E)
set a4	1.0;				# isotropic hardening parameter (see explanation under $a3)
set sigInit	0.0;				# Initial Stress Value (optional, default: 0.0)
						# the strain is calculated from epsP=$sigInit/$E
						# if (sigInit!= 0.0) { 
						# 	double epsInit = sigInit/E;
						# 	eps = trialStrain+epsInit;
						# } else eps = trialStrain; 
#--------------------------------------------------------------------------------------------------------------#
uniaxialMaterial Steel02 $steelID $Fy $Es $b $R0 $cR1 $cR2 $a1 $a2 $a3 $a4 $sigInit



#######define common variable##################################
set nfdw 30;
set nftw 6;
set nfbf 10;
set nftf 4;

source $PROC_DIR/Wsection.tcl
#format for Wsection:
#Wsection  $Tag_F_W14x22 $FrameMatID $d $bf $tf $tw $nfdw $nftw $nfbf $nftf;


####3 story hospital###########################################
#w33x152 (beam)
Wsection 5001 201 33.5 11.6 1.06 0.635 $nfdw $nftw $nfbf $nftf;
#w27x368 (column)
Wsection 6001 201 30.4 14.7 2.48 1.38 $nfdw $nftw $nfbf $nftf;





#source building_files/conn.txt
#3-HOSP												
#element	nonlinearBeamColumn	$eleTag	$iNode	$jNode	$numIntgrPts	$secTag	$transfTag	<-mass	$massDens>	<-iter	$maxIters	$tol>
# beams (RIGHT)												
element	nonlinearBeamColumn	101	4	5	3	5001	1	;	#	1		
element	nonlinearBeamColumn	102	7	8	3	5001	1	;	#	2		
element	nonlinearBeamColumn	103	10	11	3	5001	1	;	#	3		
# beams (LEFT)												
element	nonlinearBeamColumn	201	5	6	3	5001	1	;	#	1		
element	nonlinearBeamColumn	202	8	9	3	5001	1	;	#	2		
element	nonlinearBeamColumn	203	11	12	3	5001	1	;	#	3		
# columns												
element	nonlinearBeamColumn	301	1	4	5	6001	1	;	#	1	L	
element	nonlinearBeamColumn	302	3	6	5	6001	1	;	#	1	R	
element	nonlinearBeamColumn	303	4	7	5	6001	1	;	#	2	L	
element	nonlinearBeamColumn	304	6	9	5	6001	1	;	#	2	R	
element	nonlinearBeamColumn	305	7	10	5	6001	1	;	#	3	L	
element	nonlinearBeamColumn	306	9	12	5	6001	1	;	#	3	R	




file mkdir $dir; 					# create data directory

#source building_files/mass.txt ;
mass	4	0.1418	1.00E-12	1.00E-12	;
mass	6	0.1418	1.00E-12	1.00E-12	;
mass	7	0.1418	1.00E-12	1.00E-12	;
mass	9	0.1418	1.00E-12	1.00E-12	;
mass	10	0.1418	1.00E-12	1.00E-12	;
mass	12	0.1418	1.00E-12	1.00E-12	;





# Define Partition Wall Elements ----------------------------------------------------------------
set PW 0;
set PWType 0;
set IDPW 1;
set lenwall 104.0;

if {$PW == 1} {
set matID 1000;
source $PROC_DIR/pwall3.tcl
#source building_files/partitions_b.txt  ;
#PW File TYPE B						
###rigidLink $type $masterNodeTag $slaveNodeTag						
#equalDOF		2	101 1 2 3			
#equalDOF		5	102 1 2 3			
#equalDOF		5	201 1 2 3			
#equalDOF		8	202 1 2 3			
#equalDOF		8	301 1 2 3			
#equalDOF		11	302 1 2 3

rigidLink	beam	2	101			
rigidLink	beam	5	102			
rigidLink	beam	5	201			
rigidLink	beam	8	202			
rigidLink	beam	8	301			
rigidLink	beam	11	302			
						
element	zeroLength	1001	101	102	-mat $matID -dir 1 ;	
element	zeroLength	1002	201	202	-mat $matID -dir 1 ;	
element	zeroLength	1003	301	302	-mat $matID -dir 1 ;	
#equalDOF	101	102	2	3	;	
#equalDOF	201	202	2	3	;	
#equalDOF	301	302	2	3	;	
puts $lenwall;
puts "HAVE PW WALL";
} else {
  puts "NO PW WALL";    
}

# Define RECORDERS ----------------------------------------------------------------------------------------------------
recorder Node -file $dir/reacts.txt -node 1 2 3 -dof 1 2 3 reaction;
recorder Node -file $dir/reacts.txt -node 4 5 6 -dof 1 2 3 reaction;

recorder Node -file $dir/floor_acc.txt -time -dT 0.01 -node 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 48 49 50 51 52 53 54 55 56 57 58 59 60 61 62 63 -dof 1 accel;
recorder Node -file $dir/floor_disp.txt -time -dT 0.01 -node 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 48 49 50 51 52 53 54 55 56 57 58 59 60 61 62 63 -dof 1 disp;
recorder Node -file $dir/vert_floor_disp.txt -time -dT 0.01 -node 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 48 49 50 51 52 53 54 55 56 57 58 59 60 61 62 63 -dof 2 disp;
recorder Node -file $dir/floor_rot.txt -time -dT 0.01 -node 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 48 49 50 51 52 53 54 55 56 57 58 59 60 61 62 63 -dof 3 disp;

#recorder Node -file $dir/pw_acc.txt -time -dT 0.01 -node 101 102 201 202 301 302 401 402 501 502 601 602 701 702 801 802 -dof 1 accel;
#recorder Node -file $dir/pw_disp.txt -time -dT 0.01 -node 101 102 201 202 301 302 401 402 501 502 601 602 701 702 801 802 -dof 1 disp;
#recorder Node -file $dir/pw_rot.txt -time -dT 0.01 -node 101 102 201 202 301 302 401 402 501 502 601 602 701 702 801 802 -dof 3 disp;

#recorder Element -file $dir/partion_forces.txt -time -dT 0.01 -ele 1001 1002 1003 1004 1005 1006 1007 1008 force;

recorder Node -file $dir/fl_disp.txt -time -dT 0.01 -node 2 5 8 11 14 17 20 23 26 29 32 35 38 41 44 47 50 53 56 59 62 -dof 1 disp;
logFile $dir/log.txt

# determing Eigenvalues --------------------------------------------------------------------------------------------------
set outfile [open $dir/eigenvalues.txt w]
set eigenvalues [eigen 3]
puts $outfile $eigenvalues
close $outfile 
# determing EigenVectors -------------------------------------------------------------------------------------------------
#source building_files/eigen.txt

recorder Node -file $dir/eigen1.txt -node 1 4 7 10 -dof 1 "eigen 1"
recorder Node -file $dir/eigen2.txt -node 1 4 7 10 -dof 1 "eigen 2"
recorder Node -file $dir/eigen3.txt -node 1 4 7 10 -dof 1 "eigen 3"

 
# Define Gravity --------------------------------------------------------------------------------------------------------
pattern Plain 1 Linear {
#source building_files/gravity.txt
load	4	1.00E-12	-54.78	3268.8	;
load	6	1.00E-12	-54.78	-3268.8	;
load	7	1.00E-12	-54.78	3268.8	;
load	9	1.00E-12	-54.78	-3268.8	;
load	10	1.00E-12	-54.78	3268.8	;
load	12	1.00E-12	-54.78	-3268.8	;
};




# source 3HOP-no-wall-build.tcl;
puts "domain:[json-echo-domain]";
puts "disp:[json-echo-disp]";


wipe;
puts "after wipe domain:[json-echo-domain]";
