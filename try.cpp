// #include <iostream>
// #include <string>

// using namespace std;

// class car{
//     unsigned int milieage;
//     int year ;
//     string model;
//     unsigned int worth = 122;
//     string owner;

//     public:
//     int price();
//     short int  sell();

// };


// int car:: price (){
//     return worth;

// }


// template <class T>
// T sum(T s,int n){
//     T thesum = 0;

//     for (int i=0;i<n; i++){
//         thesum += s[i];
//         cout<<thesum;
//     }

//     return thesum;
// }
// int main(){
//     car lexus;

//     std::cout<<"car costs $"<<lexus.price();

//     int arr [10] = {1,4,5,6,3,5,6,4};

//     std::cout<<'\n'<<"sum: "<<sum(arr,10);

//     std::cout<<'\n'<<arr[3];
    
// }

#include <iostream>
#include <string>

using namespace std;

class Car {
    unsigned int mileage;
    int year;
    string model;
    unsigned int worth = 122;
    string owner;

public:
    int price();
    short int sell();
};

int Car::price() {
    return worth;
}

template <class T>
T sum(T* s, int n) {
    T theSum = 0;
    for (int i = 0; i < n; i++) {
        theSum += s[i],
        std::cout<<'\n'<<theSum;
    }
    return theSum;
}

int main() {
    Car lexus;

    cout << "Car costs $" << lexus.price() << endl;

    int arr[10] = {1, 4, 5, 6, 3, 5, 6, 4,5,7};

    cout << "Sum: " << sum<int>(arr, 10) << endl;

    return 0;
}
