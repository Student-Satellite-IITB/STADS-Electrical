//#include<sm_header.h>
#include<iostream>
#include<fstream>
#include<chrono>
#include<math.h>
#include<string.h>
#include<iomanip>
using namespace std;
using namespace std::chrono;

#define ANGULAR_FOV 1


    // Array to store preprocessed catalogue
    //double arr_preprocessed_catalogue[188700][4];   // SSP_ID1  SSP_ID2 Angdst_cos  Angdst_deg

    // Array to store processed reference catalogue
    unsigned int arr_ref_catalogue[188807][3]; // SSP_ID1  SSP_ID2     K_vec

    // Array to store guide catalogue
    double arr_guide_catalogue[5060][4]; // SSP_ID   x_inertial_vec     y_inertial_vec    z_inertial_vec

float comp_euclidean_distance(double x, double y)
{
    // Returns the Euclidean distance between the centre of the pixel (0, 0)
    // and the given centroids (x,y)
    // Note that it is expected that the Feature Extraction Algorithm returns
    // centroids with respect to a frame centred at the central pixel
    return (pow((x*x + y*y),0.5));
}

void sort_centroids(double arr_centroids[100][3], int Ni)
{
    /*
    INPUT:
    1. Array of centroids of stars sorted by brightness - FE_ID    x_cen   y_cen
    2. Number of Stars - Ni
    */

    // Array to copy Euclidean distance values and perform sorting
    double arr_centroids_dist[100];

    // Find Eulidean distance for all centroids from centre of sensor
    for (int i_find_dist = 0; i_find_dist < Ni; i_find_dist++)
        arr_centroids_dist[i_find_dist] = comp_euclidean_distance(arr_centroids[i_find_dist][1], arr_centroids[i_find_dist][2]);

    // Array to temporarily store arr_centroids (part of bubble sort implementation)
    double arr_centroids_tmp[4];    // FE_ID    x_cen   y_cen   eucl_dist
    // I have implemented bubble sort as it is easier but it can easily be replaced with merge sort
    for (int i_sort = 0; i_sort < (Ni-1); i_sort++)
        for (int j_sort = 0; j_sort < (Ni - i_sort - 1); j_sort++)
        {
            if (arr_centroids_dist[j_sort] > arr_centroids_dist[j_sort + 1])
            {
                // Perform swap

                arr_centroids_tmp[0] = arr_centroids[j_sort][0];
                arr_centroids_tmp[1] = arr_centroids[j_sort][1];
                arr_centroids_tmp[2] = arr_centroids[j_sort][2];
                arr_centroids_tmp[3] = arr_centroids_dist[j_sort];

                arr_centroids[j_sort][0] = arr_centroids[j_sort + 1][0];
                arr_centroids[j_sort][1] = arr_centroids[j_sort + 1][1];
                arr_centroids[j_sort][2] = arr_centroids[j_sort + 1][2];
                arr_centroids_dist[j_sort] = arr_centroids_dist[j_sort + 1];

                arr_centroids[j_sort + 1][0] = arr_centroids_tmp[0];
                arr_centroids[j_sort + 1][1] = arr_centroids_tmp[1];
                arr_centroids[j_sort + 1][2] = arr_centroids_tmp[2];
                arr_centroids_dist[j_sort + 1] = arr_centroids_tmp[3];
            }
        }

    // Overwriting all the FE_ID values as they have not been used so far
    for (int i_overwrite = 0; i_overwrite < Ni; i_overwrite++)
        arr_centroids[i_overwrite][0] = i_overwrite + 1;

    // Sorting is working as expected. VERIFIED
    cout << "Sorted array of centroids by radial distance from centre\n\n";

    /*
    OUTPUT:
    1. Array of centroids of stars sorted by radial distance from the centre of the sensor
    with overwritten FE_ID values
    */

}

void gen_3d_unit_vectors(int Ni, double arr_centroids[100][3], double arr_unit_vectors[100][5])
{
    /*
    INPUT:
    1. Ni - Number of stars
    2. arr_centroids - FE_ID    x_cen   y_cen
    3. arr_unit_vectors - FE_ID    SM_ID (not required)   x_body_vec (to be calculated here)   
                                    y_body_vec (to be calculated here)   z_body_vec (to be calculated here)
    */

    double ux, uy, uz;  // components of a unit vector
    double xu, yu;      // undistorted centroid coordinates in pixels
    double xc = 0, yc = 0;  //  pixel coordinates of the principal point
    // double ppx,ppy;     // pixel pitches of the imager
    double fmm = 36;         // focal length of the lens in mm
    // Note: Pixel_width in feature extraction is also set in mm (0.0048mm)

    // point of reference and plane of coordinates?
    // Iterative loop to assign body frame unit vectors to each centroid
    for(int i_assign_unit_vectors = 0; i_assign_unit_vectors < Ni; i_assign_unit_vectors++)
    {
        xu = arr_centroids[i_assign_unit_vectors][1];
        yu = arr_centroids[i_assign_unit_vectors][2];

        // Refer page 108 Erlank 2013
        // Advanced formula
        //ux = (xu - xc)*ppx/fmm*pow((1 + pow(((xu - xc)*ppx/fmm), 2) + pow(((yu - yc)*ppy/fmm), 2)),-0.5);
        //uy = (yu - yc)*ppy/fmm*pow((1 + pow(((xu - xc)*ppx/fmm), 2) + pow(((yu - yc)*ppy/fmm), 2)),-0.5);
        //uz = pow((1 + pow(((xu - xc)*ppx/fmm), 2) + pow(((yu - yc)*ppy/fmm), 2)),-0.5);
        // Basic formula
        ux = xu/fmm*(pow(pow(xu/fmm, 2) + pow(yu/fmm, 2) + 1,-0.5));
        uy = yu/fmm*(pow(pow(xu/fmm, 2) + pow(yu/fmm, 2) + 1,-0.5));
        uz = 1*(pow(pow(xu/fmm, 2) + pow(yu/fmm, 2) + 1,-0.5));

        arr_unit_vectors[i_assign_unit_vectors][0] = arr_centroids[i_assign_unit_vectors][0];
        arr_unit_vectors[i_assign_unit_vectors][1] = 0;
        arr_unit_vectors[i_assign_unit_vectors][2] = ux;
        arr_unit_vectors[i_assign_unit_vectors][3] = uy;
        arr_unit_vectors[i_assign_unit_vectors][4] = uz;
    }

    cout << "Generated 3D unit vectors\n\n";

    /*
    OUTPUT:
    1. arr_unit_vectors - FE_ID    SM_ID   x_body_vec (calculated here)   y_body_vec (calculated here)   z_body_vec (calculated here)
    Modified pointer (array element) values. Nothing otherwise to return
    */
}

float comp_ang_dist(double x1, double y1, double z1, double x2, double y2, double z2)
{
    // Returns angular distance values for given (x1,y1,z1) and (x2,y2,z2)
    return (pow(pow(x2-x1,2) + pow(y2-y1,2) + pow(z2-z1,2), 0.5));
}

void four_star_matching(double arr_unit_vectors[4][5])
{
    /*
    INPUT:
    1. arr_unit_vectors - FE_ID    SM_ID   x_body_vec   y_body_vec   z_body_vec
    */

    // Array to store processed reference catalogue. Removed!
    // Even if this array is not loaded onto to the compiler, it needs to be externally accessible
    // POSSIBLE ERROR!!! Mismatched understanding of elements in preprocessed catalogue. Corrected!
    //double arr_ref_catalogue[188807][3]; // SSP_ID1  SSP_ID2 K_Vec

    // Temporary variable to store Star ID
    int star_id;

    int Ngc = 5060;    // number of stars in the guide catalogue

    // Array to store values of key elements to be used in the function
    double arr_p[10];             // 0 <= i <= 5 angular distance    6 <= i <= 9 FE IDs

    // Array to export FE_IDs and corresponding SM_IDs
    int num_sm_ids;
    int arr_star_ids[4][num_sm_ids];     // FE_ID   SM_ID1  SM_ID2  SM_ID3  SM_ID4  SM_ID5 ...
    // Loop to assign zero to SM_IDs
    for(int i_set_zero = 0; i_set_zero < 4; i_set_zero++)
    {
        arr_star_ids[i_set_zero][1] = 0;
        arr_star_ids[i_set_zero][2] = 0;
        arr_star_ids[i_set_zero][3] = 0;
        arr_star_ids[i_set_zero][4] = 0;
        arr_star_ids[i_set_zero][5] = 0;
    }

    arr_star_ids[0][0] = arr_p[6] = arr_unit_vectors[0][0];   // ith star FE_ID
    arr_star_ids[1][0] = arr_p[7] = arr_unit_vectors[1][0];   // (i+1)th star FE_ID
    arr_star_ids[2][0] = arr_p[8] = arr_unit_vectors[2][0];   // (i+2)th star FE_ID
    arr_star_ids[3][0] = arr_p[9] = arr_unit_vectors[3][0];   // (i+3)th star FE_ID

    // Invoking angular distance computation function
    arr_p[0] = comp_ang_dist(arr_unit_vectors[0][2], arr_unit_vectors[0][3], arr_unit_vectors[0][4], 
                    arr_unit_vectors[1][2], arr_unit_vectors[1][3], arr_unit_vectors[1][4]);          //d12
    arr_p[1] = comp_ang_dist(arr_unit_vectors[0][2], arr_unit_vectors[0][3], arr_unit_vectors[0][4], 
                    arr_unit_vectors[2][2], arr_unit_vectors[2][3], arr_unit_vectors[2][4]);          //d13
    arr_p[2] = comp_ang_dist(arr_unit_vectors[0][2], arr_unit_vectors[0][3], arr_unit_vectors[0][4], 
                    arr_unit_vectors[3][2], arr_unit_vectors[3][3], arr_unit_vectors[3][4]);          //d14
    arr_p[3] = comp_ang_dist(arr_unit_vectors[1][2], arr_unit_vectors[1][3], arr_unit_vectors[1][4], 
                    arr_unit_vectors[2][2], arr_unit_vectors[2][3], arr_unit_vectors[2][4]);          //d23
    arr_p[4] = comp_ang_dist(arr_unit_vectors[1][2], arr_unit_vectors[1][3], arr_unit_vectors[1][4], 
                    arr_unit_vectors[3][2], arr_unit_vectors[3][3], arr_unit_vectors[3][4]);          //d24
    arr_p[5] = comp_ang_dist(arr_unit_vectors[2][2], arr_unit_vectors[2][3], arr_unit_vectors[2][4], 
                    arr_unit_vectors[3][2], arr_unit_vectors[3][3], arr_unit_vectors[3][4]);          //d34
    cout<<"Computed angular distances\n";
    // Initialise a zero Star Identification Matrix
    int arr_sim[Ngc][6];
    // Loop to set array elements to zero
    for(int i_set_zero = 0; i_set_zero < Ngc; i_set_zero++)
        for(int j_set_zero = 0; j_set_zero < 6; j_set_zero++)
            arr_sim[i_set_zero][j_set_zero] = 0;

    // 
    int n = 188807;      // Number of entries in the reference catalogue
    double ymin = 0.974001742955743000;
    double ymax = 0.999999999992621000;
    double epsilon = 2.22e-16;
    double zhi = epsilon * ymax;

    // Properties of line for k range vector matching
    double m = (ymax - ymin + 2*zhi)/(n - 1);
    cout << m << endl;
    double q = ymin - m - zhi;
    cout << q << endl;

    // Declaring scalars to define search window size
    // These also capture the essence of n(CSPA)
    double ya, yb;  // Rij_lower, Rij_upper
    int jb, jt;     // k_lower (bottom), k_upper (top)
    int kstart, kend;
    double e = 0.0002;      // Define Uncertainty Constant

    // Iterative loop to assign arr_sim values by performing k-vector search
    for(int j_cspa = 0; j_cspa < 6; j_cspa++)
    {
        if (arr_p[j_cspa] <= ANGULAR_FOV)
        {
            // To understand the approach, refer "Mortari, Neta - 2000"
            ya = arr_p[j_cspa] - e;
            yb = arr_p[j_cspa] + e;
            cout<<ya<<'\t'<<yb<<endl;
            jb = int((ya - q)/m);       // Floor function
            jt = int((yb - q)/m) + 1;   // Ceiling function
            cout<<(ya - q)/m<<'\t'<<jt<<endl;
            kstart = arr_ref_catalogue[jb][2] + 1;
            kend = arr_ref_catalogue[jt][2];
        cout<<j_cspa<<'\t'<<kstart<<'\t'<<kend<<endl;
            // Start assigning values to arr_sim
            for(int k_vector_search = kstart; k_vector_search <= kend; k_vector_search++)
            {
                arr_sim[int(arr_ref_catalogue[kstart + k_vector_search][0])][j_cspa] = 1;
                arr_sim[int(arr_ref_catalogue[kstart + k_vector_search][1])][j_cspa] = 1;
            }
        }
    }
    cout<<"Completed k vector range search\n";

    // Define Check condition. VERIFIED
    int arr_check[4][6] = { {1,1,1,0,0,0}, {1,0,0,1,1,0}, {0,1,0,1,0,1}, {0,0,1,0,1,1}};
    cout<<"Set Check condition\n";
    // Loop to find rows in arr_sim which match the arr_check matrix
    for(int j_check = 0; j_check < Ngc; j_check++)
    {
        int matched_row = -1;

        for(int i_check = 0; i_check < 4; i_check++)
        {
            for(int k_check = 0; k_check < 6; k_check++)
            {
                matched_row = i_check;
                if(arr_sim[i_check][k_check] != arr_check[i_check][k_check])
                {
                    matched_row = -1;
                    break;
                }
            }
            if(matched_row > -1)
                break;
        }  

        if(matched_row == -1)
            continue;
        else // Export all stars that match?
        {
            // Recall that j_check is iterating over the Guide Catalogue Star ID values
            for(int k_check_empty = 0; k_check_empty < num_sm_ids; k_check_empty++) 
            if(arr_star_ids[matched_row][k_check_empty] == 0)
            {   
                arr_star_ids[matched_row][k_check_empty] = j_check;
                break;
            }
            
        }
    }
    
    /*
    OUTPUT:
    1. arr_star_ids[4][num_sm_ids] - FE_ID   SM_ID1 (0 if no valid ID found)  SM_ID2    SM_ID3  SM_ID4  SM_ID5...
    Need to return this array along wih arr_unit_vectors
    */
}

void n_star_matching(double arr_unit_vectors[100][5], int Ni)
{
    /*
    INPUT:
    arr_unit_vectors - FE_ID    SM_ID   x   y   z
    Ni - Number of stars from FE
    */

    // Declaring necessary scalars
    int num_max, num_uis = Ni, num_is = 0, num_th = 8;   // Maximum number of iterations allowed, Number of unidentified stars,
                                            // Number of identified stars, Minimum number of stars required to estimate 
                                            // attitude with required attitude
    int num_match = 0, num_circ = 0;        // Number of matched stars by 4-Star Matching, Number of circular shifts in
                                            // the UIS table
    double arr_unit_vectors_new[4][5];      // Temporary array to store and pass rows from arr_unit_vectors to function
                                            // four_star_matching() - FE_ID    SM_ID   x   y   z
    short int arr_uis[100];                 // Array to keep track if a particular star has been matched or not (UIS table)
                                            // 0 means unidentified; 1 means identified; -1 means removed
    int n_circ_start = 0;                   // Attempt to circumvent need to circular shift the whole UIS table by 
                                            // introducing a variable that circulates backwards

    for (int i_4sm = 0; i_4sm < num_max; i_4sm++)
    {
        // Break loop if number of unidentified stars is less than 4 or
        // number of identified stars is greater than or equal to the threshold
        if (!((num_uis >= 4) && (num_is < num_th)))
            break;
        cout<<"Number of unidentified stars > 4\n";
        
        // Nested loop to extract 4 UIS from UIS table
        /*for (int i_extract = 0; i_extract < 4; i_extract++)
            for (int j_extract = 0; ; j_extract++)  // Condition statement given within the loop
            {
                if (arr_uis[k_4sm] == 0)
                {
                    for (int k_extract = 0; k_extract < 5; k_extract++)     // 5 in the condition statement refers to the
                                                                            // number of columns in arr_unit_vectors
                    arr_unit_vectors_new[j_extract][k_extract] = arr_unit_vectors[j_extract][k_extract];
                    k_4sm++;
                    break;  // Breaks out of j_extract loop
                }
        */

        // Loop to assign the 4 stars to be run through four_star_matching()
        for (int i_assign = 0, k_assign = n_circ_start; i_assign < 4; i_assign++)
        {
            for (int j_assign = 0; j_assign < 5; j_assign++)    // 5 in the condition statement refers to the
                                                                // number of columns in arr_unit_vectors
                arr_unit_vectors_new[i_assign][j_assign] = arr_unit_vectors[k_assign][j_assign];
            
            k_assign++;
            if (k_assign == Ni || arr_uis[k_assign] == -1)
                k_assign = 0;
        }
        cout<<"Assigned 4 stars to temporary array\n";

        // Invoke four_star_matching() over the just assigned 
        four_star_matching(arr_unit_vectors_new);
        cout<<"Four star matching run to completion\n";

        // Loop to find out the number of stars matched in four_star_matching()
        for (int i_find_num_matched = 0; i_find_num_matched < 4; i_find_num_matched++) 
        {
            if (arr_unit_vectors_new[i_find_num_matched][1] != 0)
            {
                num_match++;
                // Assigning the SM_ID to the bigger array that will be returned at the end of the function
                arr_unit_vectors[int(arr_unit_vectors_new[i_find_num_matched][0] - 1)][1] = arr_unit_vectors_new[i_find_num_matched][1];
            }
        }

        if (num_match == 0)
        {
            n_circ_start++;
            num_circ++;
            if (n_circ_start == Ni)
                n_circ_start = 0;
            if (num_circ < 2*Ni)
                break;
        }
        else
        {
            for (int i_overwrite = 0; i_overwrite < 4; i_overwrite++)
            {
                // If the star has not been matched before
                if (arr_uis[int(arr_unit_vectors_new[i_overwrite][0] - 1)] == 0)
                {
                    // Update arr_uis with -1 to signify removal
                    arr_uis[int(arr_unit_vectors_new[i_overwrite][0] - 1)] = -1;
                    // Append value to bigger array that will be returned
                    arr_unit_vectors[int(arr_unit_vectors_new[i_overwrite][0] - 1)][1] = arr_unit_vectors_new[i_overwrite][1];
                }
                // If the star has been matched before
                else
                {
                    // Overwrite new SM_ID
                    arr_unit_vectors[int(arr_unit_vectors_new[i_overwrite][0] - 1)][1] = arr_unit_vectors_new[i_overwrite][1];
                }
                
            }
            
            // Updating number of identified and number of unidentified stars
            num_is += num_match;
            num_uis -= num_match;

            // Has the required number of stars been found? / Are there any
            // unidentified stars left?
            if (num_is >= num_th || num_uis == 0)
                break;
        }

    }

    cout << "Completed n_star_matching YAYY!!\n\n";
    
}

void sm_LIS(int Ni, double arr_centroids[100][3])
{   
    // Array to store body frame unit vector values
    double arr_unit_vectors[100][5];    // FE_ID    SM_ID   x_body_vec  y_body_vec  z_body_vec

    // Sort centroids in increasing order of their radial distance from the centre of the sensor
    sort_centroids(arr_centroids, Ni);

    // Iterative loop to assign body frame unit vectors to each centroid
    gen_3d_unit_vectors(Ni, arr_centroids, arr_unit_vectors);

    // Start Voting Part
    n_star_matching(arr_unit_vectors, Ni);

    return;
}

int main()
{
    // Number of test cases
    unsigned short int n = 1;

    //arr_image and arr_centroids have been declared under the global scope to prevent stack overflow

    // Loading Reference Catalogue (Array declared globally)
    ifstream file;
    cout<<"\n...Loading Reference Star Catalogue 4SM\n";
    char filename[50] = "sm_Reference_Star_Catalogue_4SM.csv";
    file.open(filename);
    for(unsigned int i_file_in = 0; i_file_in < 188807; i_file_in++)
        for(unsigned short int j_file_in = 0; j_file_in < 4; j_file_in++)
            file >> arr_ref_catalogue[i_file_in][j_file_in];

    cout<<"Loaded Reference Star Catalogue 4SM\n\n";
    file.close();

    // Loading Guide Star Catalogue
    cout<<"...Loading Guide Star Catalogue\n";
    strcpy(filename, "sm_Guide_Star_Catalogue.csv");
    file.open(filename);
    for(unsigned int i_file_in = 0; i_file_in < 5060; i_file_in++)
        for(unsigned short int j_file_in = 0; j_file_in < 4; j_file_in++)
            file >> arr_ref_catalogue[i_file_in][j_file_in];

    cout<<"Loaded Guide Star Catalogue\n\n";
    file.close();

    
// Number of stars
int Ni = 29;
// Array of centroids
double arr_centroids[100][4];   // fe_id, x_cen, y_cen, sm_id
double arr_centroids_shrink[100][3];    // fe_id, x_cen, y_cen

//courtesy SZ
for(unsigned short img_num = 1; img_num <= n; img_num++)
	{
    //input from external file
    ifstream file;

    cout<<"Image: "<<img_num<<endl<<endl;

    char filename[100];
    sprintf(filename, "Tagging Algorithm Results/4-14/st_input_%i.txt", img_num);

    file.open(filename);
    for(unsigned short int i_file_in = 0; i_file_in < Ni; i_file_in++)
        for(unsigned short int j_file_in = 0; j_file_in < 4; j_file_in++)
            file >> arr_centroids[i_file_in][j_file_in];

    // Centroids are being imported correctly. VERIFIED
    for(unsigned short int i_arr_shrink = 0; i_arr_shrink < Ni; i_arr_shrink++)
        for(unsigned short int j_arr_shrink = 0; j_arr_shrink < 3; j_arr_shrink++)
            arr_centroids_shrink[i_arr_shrink][j_arr_shrink] = arr_centroids[i_arr_shrink][j_arr_shrink];

    //Start time
    auto start = high_resolution_clock::now();

    //Feature Extraction start
    sm_LIS(Ni, arr_centroids_shrink);
    //Feature Extraction end

    //End time
    auto stop = high_resolution_clock::now();

    //Calculate time
    auto duration = duration_cast<milliseconds>(stop - start);

    cout<<"Time Taken: "<<duration.count()<<" ms"<<endl<<endl;
    file.close();
    }

return 1;

}