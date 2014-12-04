/* obliqui2.cpp: alternative obliquity formulae

Copyright (C) 2011, Project Pluto

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
02110-1301, USA.    */

   /* This file contains some alternatives to the obliquity */
   /* formula in 'obliquit.cpp'.  That file uses a polynomial */
   /* from Meeus' _Astronomical Algorithms_ with excellent    */
   /* accuracy between -8000 to 12000.  See 'oblitest.cpp'    */
   /* for a comparison of these methods.                      */

   /* Simple cubic polynomial,  pretty good over -8000 to +12000: */
double iau_obliquity( const double t_cen);
   /* Cubic spline interpolation within values from a file.  This can */
   /* give values (if you have both of two data files) from -20 Myears */
   /* to +10 Myears.  (I _assume_ the accuracy is really much worse  */
   /* than that for very distant dates!)                             */
double file_obliquity( const double t_cen);
   /* Similar to above,  but the interpolation is done from an        */
   /* an in-memory array.  This produces identical results to the     */
   /* 'file' version,  but covers "only" -200000 to +200000 years     */
   /* around J2000.               */
double spline_obliquity( const double t_cen);

/* Following is a subset of the obliquity tables from...

http://cdsarc.u-strasbg.fr/viz-bin/Cat?VI/63
Orbital, precessional, and insolation quantities for the Earth from
-20 Myr to +10Myr
    Laskar J., Joutel F., Boudin F.
   <Astron. Astrophys. 270, 522 (1993)>
   =1993A&A...270..522L

   The original "prec0n.asc" table spans -20 million years to
the present;  "prec0p.asc" covers the present to +10 million years,
in 1000-year increments.  I just ripped out the parts from
-200000 to +200000 years;  it would obviously be straightforward
enough to grab a larger slice if desired,  but I really just
wanted to compare it to other precession methods. */

   /* t_millennia   obliquity    (radians)  precession (radians)  */
static double obliquity_table[] = {
   /*  -200.*/ 0.4037885426446118,   /* 0.1339510175094548D+01  */
   /*  -199.*/ 0.4014029192421371,   /* 0.1586640230299418D+01  */
   /*  -198.*/ 0.3992034078913982,   /* 0.1834798619105780D+01  */
   /*  -197.*/ 0.3972336017046813,   /* 0.2083884169827716D+01  */
   /*  -196.*/ 0.3955302089388765,   /* 0.2333775580625925D+01  */
   /*  -195.*/ 0.3941224026372330,   /* 0.2584335247084895D+01  */
   /*  -194.*/ 0.3930314575491552,   /* 0.2835413585419945D+01  */
   /*  -193.*/ 0.3922706668433547,   /* 0.3086853604320969D+01  */
   /*  -192.*/ 0.3918455092263272,   /*-0.2944689823359653D+01  */
   /*  -191.*/ 0.3917540257448628,   /*-0.2693004353041361D+01  */
   /*  -190.*/ 0.3919873656059647,   /*-0.2441427995120438D+01  */
   /*  -189.*/ 0.3925304674556337,   /*-0.2190104358552203D+01  */
   /*  -188.*/ 0.3933628490998685,   /*-0.1939165025066516D+01  */
   /*  -187.*/ 0.3944594774740303,   /*-0.1688727305713193D+01  */
   /*  -186.*/ 0.3957916791023747,   /*-0.1438892760683180D+01  */
   /*  -185.*/ 0.3973280334530701,   /*-0.1189746506441763D+01  */
   /*  -184.*/ 0.3990351775377368,   /*-0.9413572491876909D+00  */
   /*  -183.*/ 0.4008784516958364,   /*-0.6937778773714424D+00  */
   /*  -182.*/ 0.4028223414939910,   /*-0.4470463514946498D+00  */
   /*  -181.*/ 0.4048307175964914,   /*-0.2011865865244649D+00  */
   /*  -180.*/ 0.4068669321289857,   /* 0.4379094049661961D-01  */
   /*  -179.*/ 0.4088938770197513,   /* 0.2878890063810521D+00  */
   /*  -178.*/ 0.4108741279688113,   /* 0.5311238610633428D+00  */
   /*  -177.*/ 0.4127702768343329,   /* 0.7735256652286102D+00  */
   /*  -176.*/ 0.4145454993757426,   /* 0.1015138857715480D+01  */
   /*  -175.*/ 0.4161643322251866,   /* 0.1256022158118792D+01  */
   /*  -174.*/ 0.4175935676318083,   /* 0.1496248002593092D+01  */
   /*  -173.*/ 0.4188031393180127,   /* 0.1735901367238320D+01  */
   /*  -172.*/ 0.4197668787916150,   /* 0.1975078097534182D+01  */
   /*  -171.*/ 0.4204630645543220,   /* 0.2213882978749377D+01  */
   /*  -170.*/ 0.4208747499001514,   /* 0.2452427816684915D+01  */
   /*  -169.*/ 0.4209899159874482,   /* 0.2690829747191581D+01  */
   /*  -168.*/ 0.4208015364893035,   /* 0.2929209881513875D+01  */
   /*  -167.*/ 0.4203076489957907,   /*-0.3115493044461023D+01  */
   /*  -166.*/ 0.4195115084470201,   /*-0.2876782312112722D+01  */
   /*  -165.*/ 0.4184218595607502,   /*-0.2637715969149654D+01  */
   /*  -164.*/ 0.4170533216125710,   /*-0.2398166771213800D+01  */
   /*  -163.*/ 0.4154268406673447,   /*-0.2158009132711500D+01  */
   /*  -162.*/ 0.4135701367057120,   /*-0.1917121407365678D+01  */
   /*  -161.*/ 0.4115180560920585,   /*-0.1675388910987818D+01  */
   /*  -160.*/ 0.4093127308342338,   /*-0.1432707724896901D+01  */
   /*  -159.*/ 0.4070034429721541,   /*-0.1188989238384790D+01  */
   /*  -158.*/ 0.4046460959615538,   /*-0.9441652871637584D+00  */
   /*  -157.*/ 0.4023022090569454,   /*-0.6981936177176215D+00  */
   /*  -156.*/ 0.4000373807704586,   /*-0.4510632627955551D+00  */
   /*  -155.*/ 0.3979192169392561,   /*-0.2027992750391293D+00  */
   /*  -154.*/ 0.3960147860987865,   /* 0.4653382770597182D-01  */
   /*  -153.*/ 0.3943877411361167,   /* 0.2968305568953047D+00  */
   /*  -152.*/ 0.3930953167334913,   /* 0.5479454218359485D+00  */
   /*  -151.*/ 0.3921854591466817,   /* 0.7996966590919110D+00  */
   /*  -150.*/ 0.3916943530459567,   /* 0.1051872661915091D+01  */
   /*  -149.*/ 0.3916445722033650,   /* 0.1304240668428315D+01  */
   /*  -148.*/ 0.3920440013940864,   /* 0.1556556943089509D+01  */
   /*  -147.*/ 0.3928855727178632,   /* 0.1808577500357416D+01  */
   /*  -146.*/ 0.3941477551512818,   /* 0.2060068417978295D+01  */
   /*  -145.*/ 0.3957956562616347,   /* 0.2310814960601550D+01  */
   /*  -144.*/ 0.3977825566043267,   /* 0.2560629021802592D+01  */
   /*  -143.*/ 0.4000517042268930,   /* 0.2809354705739983D+01  */
   /*  -142.*/ 0.4025382390810887,   /* 0.3056872125620810D+01  */
   /*  -141.*/ 0.4051711756915037,   /*-0.2980085662711740D+01  */
   /*  -140.*/ 0.4078754253894146,   /*-0.2735190489462288D+01  */
   /*  -139.*/ 0.4105738697239499,   /*-0.2491631059578896D+01  */
   /*  -138.*/ 0.4131894967981779,   /*-0.2249372831172193D+01  */
   /*  -137.*/ 0.4156475856939623,   /*-0.2008345333137299D+01  */
   /*  -136.*/ 0.4178778832111104,   /*-0.1768445043196632D+01  */
   /*  -135.*/ 0.4198166786114337,   /*-0.1529539067449005D+01  */
   /*  -134.*/ 0.4214086615292320,   /*-0.1291469487555972D+01  */
   /*  -133.*/ 0.4226084551432352,   /*-0.1054058166259334D+01  */
   /*  -132.*/ 0.4233817513586616,   /*-0.8171117515395443D+00  */
   /*  -131.*/ 0.4237060279618796,   /*-0.5804266192331232D+00  */
   /*  -130.*/ 0.4235708836825826,   /*-0.3437935524084872D+00  */
   /*  -129.*/ 0.4229780684329310,   /*-0.1070020633788405D+00  */
   /*  -128.*/ 0.4219412995229169,   /* 0.1301556057457765D+00  */
   /*  -127.*/ 0.4204859361556167,   /* 0.3678806518974641D+00  */
   /*  -126.*/ 0.4186485406083307,   /* 0.6063638303525037D+00  */
   /*  -125.*/ 0.4164763008231587,   /* 0.8457813115258453D+00  */
   /*  -124.*/ 0.4140262450008719,   /* 0.1086290144417220D+01  */
   /*  -123.*/ 0.4113641602906746,   /* 0.1328023335360554D+01  */
   /*  -122.*/ 0.4085631416116406,   /* 0.1571084712331518D+01  */
   /*  -121.*/ 0.4057017382071585,   /* 0.1815543889067492D+01  */
   /*  -120.*/ 0.4028617207514982,   /* 0.2061431739069698D+01  */
   /*  -119.*/ 0.4001255439252907,   /* 0.2308736824890932D+01  */
   /*  -118.*/ 0.3975736159817093,   /* 0.2557403209189059D+01  */
   /*  -117.*/ 0.3952815044996214,   /* 0.2807330016763336D+01  */
   /*  -116.*/ 0.3933172114414114,   /* 0.3058373033747754D+01  */
   /*  -115.*/ 0.3917386497574056,   /*-0.2972836786075786D+01  */
   /*  -114.*/ 0.3905914542285366,   /*-0.2720146034601439D+01  */
   /*  -113.*/ 0.3899072596993416,   /*-0.2466982555801384D+01  */
   /*  -112.*/ 0.3897025718022222,   /*-0.2213604404366734D+01  */
   /*  -111.*/ 0.3899783282005895,   /*-0.1960274307418756D+01  */
   /*  -110.*/ 0.3907201971292523,   /*-0.1707248611547955D+01  */
   /*  -109.*/ 0.3918995898417922,   /*-0.1454766909937642D+01  */
   /*  -108.*/ 0.3934752896506145,   /*-0.1203043147112416D+01  */
   /*  -107.*/ 0.3953955416210456,   /*-0.9522587721508926D+00  */
   /*  -106.*/ 0.3976004183670721,   /*-0.7025582074115939D+00  */
   /*  -105.*/ 0.4000242831442562,   /*-0.4540465982372720D+00  */
   /*  -104.*/ 0.4025982045447609,   /*-0.2067895778608778D+00  */
   /*  -103.*/ 0.4052522231247924,   /* 0.3918534428095763D-01  */
   /*  -102.*/ 0.4079174136831955,   /* 0.2838861867762197D+00  */
   /*  -101.*/ 0.4105277166999889,   /* 0.5273530624437238D+00  */
   /*  -100.*/ 0.4130215252143140,   /* 0.7696542000191245D+00  */
   /*   -99.*/ 0.4153430129545066,   /* 0.1010881644401755D+01  */
   /*   -98.*/ 0.4174431839835416,   /* 0.1251146812766256D+01  */
   /*   -97.*/ 0.4192806221972361,   /* 0.1490576064620303D+01  */
   /*   -96.*/ 0.4208219266296318,   /* 0.1729306440547494D+01  */
   /*   -95.*/ 0.4220418369550156,   /* 0.1967481721032381D+01  */
   /*   -94.*/ 0.4229230794241352,   /* 0.2205248939363528D+01  */
   /*   -93.*/ 0.4234559900919690,   /* 0.2442755443684477D+01  */
   /*   -92.*/ 0.4236379919781642,   /* 0.2680146543830155D+01  */
   /*   -91.*/ 0.4234730095304949,   /* 0.2917563707466080D+01  */
   /*   -90.*/ 0.4229708944563522,   /*-0.3128042105826061D+01  */
   /*   -89.*/ 0.4221469128921307,   /*-0.2890170284106579D+01  */
   /*   -88.*/ 0.4210213104050873,   /*-0.2651883357612840D+01  */
   /*   -87.*/ 0.4196189369065240,   /*-0.2413066758857398D+01  */
   /*   -86.*/ 0.4179688875872035,   /*-0.2173615254686386D+01  */
   /*   -85.*/ 0.4161041061546177,   /*-0.1933434227108985D+01  */
   /*   -84.*/ 0.4140609063818383,   /*-0.1692441048821370D+01  */
   /*   -83.*/ 0.4118783947630255,   /*-0.1450566433088198D+01  */
   /*   -82.*/ 0.4095978124589096,   /*-0.1207755621150562D+01  */
   /*   -81.*/ 0.4072618463055355,   /*-0.9639693053546469D+00  */
   /*   -80.*/ 0.4049139738632165,   /*-0.7191842645058081D+00  */
   /*   -79.*/ 0.4025978980796790,   /*-0.4733937834681989D+00  */
   /*   -78.*/ 0.4003570931142296,   /*-0.2266080038682889D+00  */
   /*   -77.*/ 0.3982344338462959,   /* 0.2114563030390704D-01  */
   /*   -76.*/ 0.3962718347171669,   /* 0.2698217304426851D+00  */
   /*   -75.*/ 0.3945097978351587,   /* 0.5193561560972624D+00  */
   /*   -74.*/ 0.3929867790632433,   /* 0.7696652401874184D+00  */
   /*   -73.*/ 0.3917383254419973,   /* 0.1020645382372826D+01  */
   /*   -72.*/ 0.3907960050842220,   /* 0.1272173398933720D+01  */
   /*   -71.*/ 0.3901862188823471,   /* 0.1524107953306865D+01  */
   /*   -70.*/ 0.3899290281566938,   /* 0.1776292222399513D+01  */
   /*   -69.*/ 0.3900371388752039,   /* 0.2028557733147899D+01  */
   /*   -68.*/ 0.3905151518157988,   /* 0.2280729104055514D+01  */
   /*   -67.*/ 0.3913591343081909,   /* 0.2532629307775426D+01  */
   /*   -66.*/ 0.3925565154137390,   /* 0.2784085055099727D+01  */
   /*   -65.*/ 0.3940862713065346,   /* 0.3034931965802242D+01  */
   /*   -64.*/ 0.3959193578225051,   /*-0.2998166018760701D+01  */
   /*   -63.*/ 0.3980193556084838,   /*-0.2748971294741219D+01  */
   /*   -62.*/ 0.4003433052031937,   /*-0.2500781044963122D+01  */
   /*   -61.*/ 0.4028427116175390,   /*-0.2253683423665604D+01  */
   /*   -60.*/ 0.4054646866540161,   /*-0.2007741148070215D+01  */
   /*   -59.*/ 0.4081531789745269,   /*-0.1762990665463898D+01  */
   /*   -58.*/ 0.4108502291362232,   /*-0.1519442136418256D+01  */
   /*   -57.*/ 0.4134971898427233,   /*-0.1277080084830247D+01  */
   /*   -56.*/ 0.4160358729710453,   /*-0.1035864509865132D+01  */
   /*   -55.*/ 0.4184096177485623,   /*-0.7957322502031227D+00  */
   /*   -54.*/ 0.4205643060684390,   /*-0.5565984413329632D+00  */
   /*   -53.*/ 0.4224493686082863,   /*-0.3183579949062421D+00  */
   /*   -52.*/ 0.4240188218582427,   /*-0.8088712504362428D-01  */
   /*   -51.*/ 0.4252323523015520,   /* 0.1559549810751986D+00  */
   /*   -50.*/ 0.4260564282353656,   /* 0.3923242224756961D+00  */
   /*   -49.*/ 0.4264653844159292,   /* 0.6283892670514071D+00  */
   /*   -48.*/ 0.4264424015063243,   /* 0.8643288068569152D+00  */
   /*   -47.*/ 0.4259802982916808,   /* 0.1100328405721968D+01  */
   /*   -46.*/ 0.4250820704900247,   /* 0.1336577020609500D+01  */
   /*   -45.*/ 0.4237611403737662,   /* 0.1573263312614944D+01  */
   /*   -44.*/ 0.4220413170053153,   /* 0.1810571861520689D+01  */
   /*   -43.*/ 0.4199564972537426,   /* 0.2048679360993549D+01  */
   /*   -42.*/ 0.4175501542075140,   /* 0.2287750811440860D+01  */
   /*   -41.*/ 0.4148746574007646,   /* 0.2527935663353095D+01  */
   /*   -40.*/ 0.4119904489174027,   /* 0.2769363818701827D+01  */
   /*   -39.*/ 0.4089650667384774,   /* 0.3012141393999119D+01  */
   /*   -38.*/ 0.4058719716751814,   /*-0.3026839104828821D+01  */
   /*   -37.*/ 0.4027891089243749,   /*-0.2781162278958302D+01  */
   /*   -36.*/ 0.3997971308357887,   /*-0.2534006370516114D+01  */
   /*   -35.*/ 0.3969772310218505,   /*-0.2285406257945027D+01  */
   /*   -34.*/ 0.3944085919953214,   /*-0.2035441920990531D+01  */
   /*   -33.*/ 0.3921655216481440,   /*-0.1784239842618717D+01  */
   /*   -32.*/ 0.3903144332768422,   /*-0.1531972001913010D+01  */
   /*   -31.*/ 0.3889108902794321,   /*-0.1278851994083649D+01  */
   /*   -30.*/ 0.3879969715113198,   /*-0.1025128143225890D+01  */
   /*   -29.*/ 0.3875992046384497,   /*-0.7710738860230836D+00  */
   /*   -28.*/ 0.3877272620443951,   /*-0.5169761094581331D+00  */
   /*   -27.*/ 0.3883735287254252,   /*-0.2631224259718647D+00  */
   /*   -26.*/ 0.3895135546374215,   /*-0.9788496389462916D-02  */
   /*   -25.*/ 0.3911073169630848,   /* 0.2427735526946539D+00  */
   /*   -24.*/ 0.3931011561132117,   /* 0.4943447874864249D+00  */
   /*   -23.*/ 0.3954302174083974,   /* 0.7447466718600525D+00  */
   /*   -22.*/ 0.3980212227039923,   /* 0.9938455982557106D+00  */
   /*   -21.*/ 0.4007954023543951,   /* 0.1241554791835688D+01  */
   /*   -20.*/ 0.4036714292374652,   /* 0.1487833839494220D+01  */
   /*   -19.*/ 0.4065682103583174,   /* 0.1732686237442576D+01  */
   /*   -18.*/ 0.4094074108960788,   /* 0.1976155439034697D+01  */
   /*   -17.*/ 0.4121156154125473,   /* 0.2218319922018747D+01  */
   /*   -16.*/ 0.4146260729802799,   /* 0.2459287778869943D+01  */
   /*   -15.*/ 0.4168800222221041,   /* 0.2699191262710689D+01  */
   /*   -14.*/ 0.4188276375966949,   /* 0.2938181601266707D+01  */
   /*   -13.*/ 0.4204286669980098,   /*-0.3106761063672589D+01  */
   /*   -12.*/ 0.4216528346605714,   /*-0.2869090746452861D+01  */
   /*   -11.*/ 0.4224800633281588,   /*-0.2631811387522883D+01  */
   /*   -10.*/ 0.4229005359258803,   /*-0.2394739305292919D+01  */
   /*    -9.*/ 0.4229145851278091,   /*-0.2157692044100071D+01  */
   /*    -8.*/ 0.4225323833330887,   /*-0.1920491819747929D+01  */
   /*    -7.*/ 0.4217734121543107,   /*-0.1682968842703293D+01  */
   /*    -6.*/ 0.4206657157453829,   /*-0.1444964443881118D+01  */
   /*    -5.*/ 0.4192449738329285,   /*-0.1206333923966576D+01  */
   /*    -4.*/ 0.4175534529720676,   /*-0.9669490865584757D+00  */
   /*    -3.*/ 0.4156388971064635,   /*-0.7267004768910357D+00  */
   /*    -2.*/ 0.4135533987884533,   /*-0.4854993986755168D+00  */
   /*    -1.*/ 0.4113522583107795,   /*-0.2432797910064849D+00  */
   /*     0.*/ 0.4090928042223416,   /* 0.0000000000000000D+00  */
   /*     1.*/ 0.4068331306224137,   /* 0.2443556166147563D+00  */
   /*     2.*/ 0.4046307140843110,   /* 0.4897754299502705D+00  */
   /*     3.*/ 0.4025409062755741,   /* 0.7362198976873164D+00  */
   /*     4.*/ 0.4006153476974149,   /* 0.9836218435742742D+00  */
   /*     5.*/ 0.3989003972459766,   /* 0.1231887910097264D+01  */
   /*     6.*/ 0.3974357043804754,   /* 0.1480901300404421D+01  */
   /*     7.*/ 0.3962530535517125,   /* 0.1730525738907529D+01  */
   /*     8.*/ 0.3953755816010525,   /* 0.1980610391924189D+01  */
   /*     9.*/ 0.3948174158666943,   /* 0.2230995352572876D+01  */
   /*    10.*/ 0.3945837191886220,   /* 0.2481517244187717D+01  */
   /*    11.*/ 0.3946710755773147,   /* 0.2732014541182347D+01  */
   /*    12.*/ 0.3950681208246240,   /* 0.2982332322324208D+01  */
   /*    13.*/ 0.3957563212685792,   /*-0.3050858992199093D+01  */
   /*    14.*/ 0.3967108271244377,   /*-0.2801319094117513D+01  */
   /*    15.*/ 0.3979013626107844,   /*-0.2552346984074016D+01  */
   /*    16.*/ 0.3992931489193319,   /*-0.2304037703751810D+01  */
   /*    17.*/ 0.4008478755146728,   /*-0.2056466181013103D+01  */
   /*    18.*/ 0.4025247342047322,   /*-0.1809686364813078D+01  */
   /*    19.*/ 0.4042815107640464,   /*-0.1563731008137131D+01  */
   /*    20.*/ 0.4060756993806367,   /*-0.1318612140790998D+01  */
   /*    21.*/ 0.4078655781426910,   /*-0.1074322220583949D+01  */
   /*    22.*/ 0.4096111706400367,   /*-0.8308358691276224D+00  */
   /*    23.*/ 0.4112750261617907,   /*-0.5881120142768895D+00  */
   /*    24.*/ 0.4128227784927114,   /*-0.3460962042650276D+00  */
   /*    25.*/ 0.4142234836000313,   /*-0.1047228497557210D+00  */
   /*    26.*/ 0.4154497777284317,   /* 0.1360828047621590D+00  */
   /*    27.*/ 0.4164779271557997,   /* 0.3764030905014157D+00  */
   /*    28.*/ 0.4172878502067638,   /* 0.6163267079279625D+00  */
   /*    29.*/ 0.4178631787524808,   /* 0.8559478113817843D+00  */
   /*    30.*/ 0.4181913955356478,   /* 0.1095365232926585D+01  */
   /*    31.*/ 0.4182640462891083,   /* 0.1334681685360392D+01  */
   /*    32.*/ 0.4180769947246574,   /* 0.1574002822845911D+01  */
   /*    33.*/ 0.4176306740187046,   /* 0.1813436102844365D+01  */
   /*    34.*/ 0.4169302933654310,   /* 0.2053089456292002D+01  */
   /*    35.*/ 0.4159859771766924,   /* 0.2293069808431037D+01  */
   /*    36.*/ 0.4148128362904114,   /* 0.2533481488346331D+01  */
   /*    37.*/ 0.4134309829982735,   /* 0.2774424528322483D+01  */
   /*    38.*/ 0.4118654976707642,   /* 0.3015992808388241D+01  */
   /*    39.*/ 0.4101463357263359,   /*-0.3024913330602498D+01  */
   /*    40.*/ 0.4083081394031458,   /*-0.2781848214031362D+01  */
   /*    41.*/ 0.4063899027014311,   /*-0.2537935297606801D+01  */
   /*    42.*/ 0.4044344403646262,   /*-0.2293128726410040D+01  */
   /*    43.*/ 0.4024876344847781,   /*-0.2047401418800972D+01  */
   /*    44.*/ 0.4005974666244765,   /*-0.1800747614461007D+01  */
   /*    45.*/ 0.3988128743632460,   /*-0.1553184987937212D+01  */
   /*    46.*/ 0.3971824855115845,   /*-0.1304756171319349D+01  */
   /*    47.*/ 0.3957532764747999,   /*-0.1055529583150466D+01  */
   /*    48.*/ 0.3945691809094172,   /*-0.8055994839071512D+00  */
   /*    49.*/ 0.3936696568430963,   /*-0.5550851576863607D+00  */
   /*    50.*/ 0.3930882208334509,   /*-0.3041290735837638D+00  */
   /*    51.*/ 0.3928509836446831,   /*-0.5289385348489109D-01  */
   /*    52.*/ 0.3929752667351771,   /* 0.1984420852290875D+00  */
   /*    53.*/ 0.3934684241855990,   /* 0.4496902076101385D+00  */
   /*    54.*/ 0.3943270183092096,   /* 0.7006586421142208D+00  */
   /*    55.*/ 0.3955364831922726,   /* 0.9511596152675641D+00  */
   /*    56.*/ 0.3970713568729473,   /* 0.1201016918161661D+01  */
   /*    57.*/ 0.3988960831120016,   /* 0.1450072733407353D+01  */
   /*    58.*/ 0.4009663009430399,   /* 0.1698193246750059D+01  */
   /*    59.*/ 0.4032304782061602,   /* 0.1945272680096612D+01  */
   /*    60.*/ 0.4056317196870841,   /* 0.2191235647566864D+01  */
   /*    61.*/ 0.4081095941243174,   /* 0.2436037979137428D+01  */
   /*    62.*/ 0.4106018679176829,   /* 0.2679666320375329D+01  */
   /*    63.*/ 0.4130460901994423,   /* 0.2922136877163754D+01  */
   /*    64.*/ 0.4153810262203307,   /*-0.3119691666838704D+01  */
   /*    65.*/ 0.4175479702703095,   /*-0.2879378977649773D+01  */
   /*    66.*/ 0.4194919795713276,   /*-0.2640017127489179D+01  */
   /*    67.*/ 0.4211630586804311,   /*-0.2401491712321727D+01  */
   /*    68.*/ 0.4225172980342602,   /*-0.2163669672984704D+01  */
   /*    69.*/ 0.4235179414023910,   /*-0.1926402025130027D+01  */
   /*    70.*/ 0.4241363355894057,   /*-0.1689526829649581D+01  */
   /*    71.*/ 0.4243527085098502,   /*-0.1452872384966725D+01  */
   /*    72.*/ 0.4241567303539652,   /*-0.1216260580675402D+01  */
   /*    73.*/ 0.4235478334654247,   /*-0.9795103282517400D+00  */
   /*    74.*/ 0.4225352926410604,   /*-0.7424409882597867D+00  */
   /*    75.*/ 0.4211380903815376,   /*-0.5048757446124233D+00  */
   /*    76.*/ 0.4193846038503792,   /*-0.2666449261647262D+00  */
   /*    77.*/ 0.4173121478346718,   /*-0.2758932863770884D-01  */
   /*    78.*/ 0.4149663912585334,   /* 0.2124363732195464D+00  */
   /*    79.*/ 0.4124006389033184,   /* 0.4535600297618344D+00  */
   /*    80.*/ 0.4096749436630418,   /* 0.6958881844143646D+00  */
   /*    81.*/ 0.4068549978794650,   /* 0.9395020389540526D+00  */
   /*    82.*/ 0.4040107531129969,   /* 0.1184453367540253D+01  */
   /*    83.*/ 0.4012147392298109,   /* 0.1430760655982383D+01  */
   /*    84.*/ 0.3985400924104437,   /* 0.1678405843684044D+01  */
   /*    85.*/ 0.3960583480967850,   /* 0.1927332094227704D+01  */
   /*    86.*/ 0.3938370966381299,   /* 0.2177443000786976D+01  */
   /*    87.*/ 0.3919376260275518,   /* 0.2428603546750192D+01  */
   /*    88.*/ 0.3904126834388647,   /* 0.2680643008723552D+01  */
   /*    89.*/ 0.3893044789354861,   /* 0.2933359833804336D+01  */
   /*    90.*/ 0.3886430397161544,   /*-0.3096656942093088D+01  */
   /*    91.*/ 0.3884450100590401,   /*-0.2843278172374169D+01  */
   /*    92.*/ 0.3887129826219560,   /*-0.2589937016873094D+01  */
   /*    93.*/ 0.3894354340775427,   /*-0.2336877740361646D+01  */
   /*    94.*/ 0.3905873104924948,   /*-0.2084331211489573D+01  */
   /*    95.*/ 0.3921312572599651,   /*-0.1832506026276891D+01  */
   /*    96.*/ 0.3940194180073068,   /*-0.1581581271407524D+01  */
   /*    97.*/ 0.3961956531522340,   /*-0.1331701442505301D+01  */
   /*    98.*/ 0.3985979751817953,   /*-0.1082973733897298D+01  */
   /*    99.*/ 0.4011609839328693,   /*-0.8354675832113583D+00  */
   /*   100.*/ 0.4038181172056783,   /*-0.5892160743901576D+00  */
   /*   101.*/ 0.4065035990919956,   /*-0.3442186450908055D+00  */
   /*   102.*/ 0.4091540473916359,   /*-0.1004445326578767D+00  */
   /*   103.*/ 0.4117097671429156,   /* 0.1421634990364562D+00  */
   /*   104.*/ 0.4141157922931505,   /* 0.3836854408402384D+00  */
   /*   105.*/ 0.4163227383095657,   /* 0.6242204527748144D+00  */
   /*   106.*/ 0.4182875045720320,   /* 0.8638834253217347D+00  */
   /*   107.*/ 0.4199738333870426,   /* 0.1102801658831977D+01  */
   /*   108.*/ 0.4213527085904940,   /* 0.1341111647012224D+01  */
   /*   109.*/ 0.4224025705700182,   /* 0.1578955992778419D+01  */
   /*   110.*/ 0.4231093368122283,   /* 0.1816480523027157D+01  */
   /*   111.*/ 0.4234662409114051,   /* 0.2053831680676985D+01  */
   /*   112.*/ 0.4234735277784344,   /* 0.2291154250293499D+01  */
   /*   113.*/ 0.4231380586184182,   /* 0.2528589424843650D+01  */
   /*   114.*/ 0.4224728802038215,   /* 0.2766273162342554D+01  */
   /*   115.*/ 0.4214967987433137,   /* 0.3004334731870390D+01  */
   /*   116.*/ 0.4202339740339891,   /*-0.3040289982688469D+01  */
   /*   117.*/ 0.4187135224730843,   /*-0.2801118693454088D+01  */
   /*   118.*/ 0.4169690961382974,   /*-0.2561236116718987D+01  */
   /*   119.*/ 0.4150383956077978,   /*-0.2320554449046042D+01  */
   /*   120.*/ 0.4129625788017915,   /*-0.2079000466958799D+01  */
   /*   121.*/ 0.4107855450518595,   /*-0.1836517300006162D+01  */
   /*   122.*/ 0.4085530976060879,   /*-0.1593066046199709D+01  */
   /*   123.*/ 0.4063120118034033,   /*-0.1348627091061239D+01  */
   /*   124.*/ 0.4041090533340783,   /*-0.1103201019437420D+01  */
   /*   125.*/ 0.4019899966373665,   /*-0.8568090556093901D+00  */
   /*   126.*/ 0.3999986864808658,   /*-0.6094930165136480D+00  */
   /*   127.*/ 0.3981761691340588,   /*-0.3613147988120235D+00  */
   /*   128.*/ 0.3965598996109067,   /*-0.1123554318723623D+00  */
   /*   129.*/ 0.3951830158216477,   /* 0.1372862858511940D+00  */
   /*   130.*/ 0.3940736653411686,   /* 0.3874955796921174D+00  */
   /*   131.*/ 0.3932543781833389,   /* 0.6381439602493516D+00  */
   /*   132.*/ 0.3927414966879600,   /* 0.8890919956226929D+00  */
   /*   133.*/ 0.3925446941940581,   /* 0.1140192602733037D+01  */
   /*   134.*/ 0.3926666285652076,   /* 0.1391294797751150D+01  */
   /*   135.*/ 0.3931027776267974,   /* 0.1642247761377218D+01  */
   /*   136.*/ 0.3938414890354592,   /* 0.1892905003320007D+01  */
   /*   137.*/ 0.3948642513986466,   /* 0.2143128379203586D+01  */
   /*   138.*/ 0.3961461660713683,   /* 0.2392791735432366D+01  */
   /*   139.*/ 0.3976565805499599,   /* 0.2641784024899493D+01  */
   /*   140.*/ 0.3993598414282442,   /* 0.2890011821638704D+01  */
   /*   141.*/ 0.4012161366630831,   /* 0.3137401232012463D+01  */
   /*   142.*/ 0.4031824150415920,   /*-0.2899286076932724D+01  */
   /*   143.*/ 0.4052133832237458,   /*-0.2653710871754694D+01  */
   /*   144.*/ 0.4072625780763823,   /*-0.2409067990768375D+01  */
   /*   145.*/ 0.4092834927031928,   /*-0.2165345514234318D+01  */
   /*   146.*/ 0.4112307067195019,   /*-0.1922511085716443D+01  */
   /*   147.*/ 0.4130609494043810,   /*-0.1680513488932353D+01  */
   /*   148.*/ 0.4147340223878986,   /*-0.1439284647229781D+01  */
   /*   149.*/ 0.4162135327393912,   /*-0.1198741832272754D+01  */
   /*   150.*/ 0.4174674321213753,   /*-0.9587898466278730D+00  */
   /*   151.*/ 0.4184684070546409,   /*-0.7193229936990948D+00  */
   /*   152.*/ 0.4191941994813850,   /*-0.4802267584488698D+00  */
   /*   153.*/ 0.4196279409163138,   /*-0.2413792580818679D+00  */
   /*   154.*/ 0.4197585545731583,   /*-0.2652636193900383D-02  */
   /*   155.*/ 0.4195812282688133,   /* 0.2360853725923978D+00  */
   /*   156.*/ 0.4190979057668582,   /* 0.4749695039202526D+00  */
   /*   157.*/ 0.4183177053671864,   /* 0.7141345356686158D+00  */
   /*   158.*/ 0.4172571646732103,   /* 0.9537123750222740D+00  */
   /*   159.*/ 0.4159402308041304,   /* 0.1193828775360574D+01  */
   /*   160.*/ 0.4143979562373412,   /* 0.1434599866151028D+01  */
   /*   161.*/ 0.4126679061850000,   /* 0.1676128692149758D+01  */
   /*   162.*/ 0.4107933185158665,   /* 0.1918501924043020D+01  */
   /*   163.*/ 0.4088220723229434,   /* 0.2161786845595691D+01  */
   /*   164.*/ 0.4068055156587129,   /* 0.2406028672562051D+01  */
   /*   165.*/ 0.4047971840033980,   /* 0.2651248240309658D+01  */
   /*   166.*/ 0.4028514202965486,   /* 0.2897440119852649D+01  */
   /*   167.*/ 0.4010218959888728,   /*-0.3138614029761584D+01  */
   /*   168.*/ 0.3993600371834810,   /*-0.2890604849660533D+01  */
   /*   169.*/ 0.3979133805880060,   /*-0.2641806793560400D+01  */
   /*   170.*/ 0.3967239145739813,   /*-0.2392335400132350D+01  */
   /*   171.*/ 0.3958264912782198,   /*-0.2142329777348108D+01  */
   /*   172.*/ 0.3952474162542253,   /*-0.1891948422446297D+01  */
   /*   173.*/ 0.3950033255861159,   /*-0.1641363809132225D+01  */
   /*   174.*/ 0.3951004445907086,   /*-0.1390756025786525D+01  */
   /*   175.*/ 0.3955342905536659,   /*-0.1140305874230724D+01  */
   /*   176.*/ 0.3962898415108828,   /*-0.8901879045573635D+00  */
   /*   177.*/ 0.3973421523769988,   /*-0.6405638574385065D+00  */
   /*   178.*/ 0.3986573660100075,   /*-0.3915769183297437D+00  */
   /*   179.*/ 0.4001940443743395,   /*-0.1433470784603047D+00  */
   /*   180.*/ 0.4019047345415215,   /* 0.1040322285444517D+00  */
   /*   181.*/ 0.4037376835332590,   /* 0.3504961644861406D+00  */
   /*   182.*/ 0.4056386209691270,   /* 0.5960092453166138D+00  */
   /*   183.*/ 0.4075525351607142,   /* 0.8405649360181059D+00  */
   /*   184.*/ 0.4094253743325376,   /* 0.1084184241851686D+01  */
   /*   185.*/ 0.4112056102067528,   /* 0.1326913485271506D+01  */
   /*   186.*/ 0.4128456087605856,   /* 0.1568821477773958D+01  */
   /*   187.*/ 0.4143027659792556,   /* 0.1809996308164642D+01  */
   /*   188.*/ 0.4155403870327739,   /* 0.2050541968150751D+01  */
   /*   189.*/ 0.4165283143167994,   /* 0.2290575014205324D+01  */
   /*   190.*/ 0.4172433378940939,   /* 0.2530221414702089D+01  */
   /*   191.*/ 0.4176694427620714,   /* 0.2769613654611978D+01  */
   /*   192.*/ 0.4177979527762254,   /* 0.3008888078885497D+01  */
   /*   193.*/ 0.4176276164551140,   /*-0.3035002935109985D+01  */
   /*   194.*/ 0.4171646474152031,   /*-0.2795552285742719D+01  */
   /*   195.*/ 0.4164226912490700,   /*-0.2555812692955428D+01  */
   /*   196.*/ 0.4154226556232529,   /*-0.2315658424736075D+01  */
   /*   197.*/ 0.4141923256760832,   /*-0.2074973789539310D+01  */
   /*   198.*/ 0.4127657011969876,   /*-0.1833656405749656D+01  */
   /*   199.*/ 0.4111820342269412,   /*-0.1591620312189744D+01  */
   /*   200.*/ 0.4094846033468814,   /*-0.1348798640860899D+01  */
   };

// #include "watdefs.h"
// #include "lunar.h"

#define PI 3.1415926535897932384626433832795028841971693993751058209749445923
#define CVT (PI / 180.)
#define ARCSECONDS_TO_RADIANS (CVT / 3600.)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* Copied from (22.2),  Meeus,  p. 147;  he,  in turn,  got it
from _Astronomical Almanac_ 1984,  p. S26.   */

double iau_obliquity( const double t_cen)
{
   const double rval =
           (23. * 3600. + 26. * 60 + 21.448) * ARCSECONDS_TO_RADIANS
           + t_cen * (-46.8150 * ARCSECONDS_TO_RADIANS   /* linear */
           + t_cen * (-0.00059 * ARCSECONDS_TO_RADIANS   /* quad */
           + t_cen * 0.001813 * ARCSECONDS_TO_RADIANS));

   return( rval);
}

double cubic_spline_interpolate_within_table(      /* spline.cpp */
         const double *table, const int n_entries, double x, int *err_code);


double spline_obliquity( const double t_cen)
{
   const double table_start = -2000.;   /* table starts at 2000    */
                                        /* centuries before J2000  */

   return( cubic_spline_interpolate_within_table(
            obliquity_table,
            sizeof( obliquity_table) / sizeof( double),
            (t_cen - table_start) / 10., NULL));
}

/* The following does a cubic interpolation within values from Laskar
et. al. (same as in the above obliquity_table[],  except that the
files cover about a time span about 75 times greater.)  Note that
file_obliquity( ) is somewhat complicated by the fact that Laskar
et. al. provided data split between two files.  Rather than read
from both files when in the "transition" zone at J2000 +/- one
millennium,  I just stored values for the years 1000 and 3000.  */

double file_obliquity( const double t_cen)
{
   FILE *ifile = fopen( t_cen < 0. ? "prec0n.asc" : "prec0p.asc", "rb");
   double rval = 0.;

   if( ifile)
      {
      char buff[80];

      if( fgets( buff, sizeof( buff), ifile))
         {
         double loc = fabs( t_cen / 10.);   /* one record per millenium */
         int iloc = (int)loc - 1, i, err = 0;
         double array[4];

         for( i = 0; i < 4; i++, iloc++)
            {
            const double obliquit_minus_1k = 0.4113522583107795;
            const double obliquit_plus_1k =  0.4068331306224137;

            if( iloc == -1)
               array[i] = (t_cen < 0. ? obliquit_plus_1k : obliquit_minus_1k);
            else
               {
               fseek( ifile, (size_t)iloc * strlen( buff), SEEK_SET);
               if( fgets( buff, sizeof( buff), ifile))
                  array[i] = atof( buff + 11);
               else
                  err = -99;
               }
            }
         if( !err)
            rval = cubic_spline_interpolate_within_table(
                  array, 4, loc - iloc + 4., &err);
         if( err)
            printf( "Interpolation: %d\n", err);
         }
      fclose( ifile);
      }
   return( rval);
}