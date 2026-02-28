/**
 * Exercise 1: 배열 합계 계산기
 *
 * 학습 목표:
 * - 포인터를 함수에 전달하기
 * - 동적 배열 할당/해제
 * - 메모리 안전성 (nullptr 확인)
 *
 * 난이도: ⭐ 초급
 */

#include <iostream>
#include <iomanip>
#include <cstdlib>

using namespace std;

/**
 * 배열의 합계를 계산하는 함수
 *
 * @param arr: 정수 배열 포인터
 * @param size: 배열 크기
 * @return: 배열의 합계
 */
long long calculateSum(const int* arr, int size) {
    if (arr == nullptr) {
        cout << "오류: 배열이 null입니다" << endl;
        return 0;
    }

    if (size <= 0) {
        cout << "오류: 배열 크기가 0 이하입니다" << endl;
        return 0;
    }

    long long sum = 0;
    for (int i = 0; i < size; i++) {
        sum += arr[i];
    }
    return sum;
}

/**
 * 배열의 평균을 계산하는 함수
 */
double calculateAverage(const int* arr, int size) {
    if (size <= 0) return 0;
    return (double)calculateSum(arr, size) / size;
}

/**
 * 배열의 최댓값을 찾는 함수
 */
int findMax(const int* arr, int size) {
    if (arr == nullptr || size <= 0) {
        return INT_MIN;
    }

    int max = arr[0];
    for (int i = 1; i < size; i++) {
        if (arr[i] > max) {
            max = arr[i];
        }
    }
    return max;
}

/**
 * 배열의 최솟값을 찾는 함수
 */
int findMin(const int* arr, int size) {
    if (arr == nullptr || size <= 0) {
        return INT_MAX;
    }

    int min = arr[0];
    for (int i = 1; i < size; i++) {
        if (arr[i] < min) {
            min = arr[i];
        }
    }
    return min;
}

/**
 * 배열을 출력하는 함수
 */
void printArray(const int* arr, int size, int maxElements = 10) {
    if (arr == nullptr) {
        cout << "배열이 null입니다" << endl;
        return;
    }

    cout << "[ ";
    for (int i = 0; i < size && i < maxElements; i++) {
        cout << arr[i];
        if (i < size - 1) cout << ", ";
    }
    if (size > maxElements) {
        cout << ", ... (총 " << size << "개)";
    }
    cout << " ]" << endl;
}

int main() {
    cout << "╔═══════════════════════════════════════╗" << endl;
    cout << "║  Exercise 1: 배열 합계 계산기          ║" << endl;
    cout << "╚═══════════════════════════════════════╝" << endl << endl;

    // 테스트 1: 작은 배열
    {
        cout << "📝 테스트 1: 작은 배열 (10개)" << endl;
        int size = 10;
        int* arr = new int[size];

        // 데이터 채우기
        for (int i = 0; i < size; i++) {
            arr[i] = (i + 1) * 10;  // 10, 20, 30, ..., 100
        }

        cout << "배열: ";
        printArray(arr, size);

        long long sum = calculateSum(arr, size);
        double avg = calculateAverage(arr, size);
        int max = findMax(arr, size);
        int min = findMin(arr, size);

        cout << "합계: " << sum << endl;
        cout << "평균: " << fixed << setprecision(2) << avg << endl;
        cout << "최댓값: " << max << endl;
        cout << "최솟값: " << min << endl;

        delete[] arr;
        arr = nullptr;
        cout << "✓ 메모리 해제 완료" << endl << endl;
    }

    // 테스트 2: 큰 배열
    {
        cout << "📝 테스트 2: 큰 배열 (1000개)" << endl;
        int size = 1000;
        int* arr = new int[size];

        // 랜덤 데이터
        srand(42);  // 시드 고정 (재현성)
        for (int i = 0; i < size; i++) {
            arr[i] = rand() % 100;
        }

        long long sum = calculateSum(arr, size);
        double avg = calculateAverage(arr, size);
        int max = findMax(arr, size);
        int min = findMin(arr, size);

        cout << "합계: " << sum << endl;
        cout << "평균: " << fixed << setprecision(2) << avg << endl;
        cout << "최댓값: " << max << endl;
        cout << "최솟값: " << min << endl;

        delete[] arr;
        arr = nullptr;
        cout << "✓ 메모리 해제 완료" << endl << endl;
    }

    // 테스트 3: 음수 포함
    {
        cout << "📝 테스트 3: 음수 포함 배열" << endl;
        int arr[] = {-5, 10, -3, 20, -8, 15, 2};
        int size = 7;

        cout << "배열: ";
        printArray(arr, size);

        long long sum = calculateSum(arr, size);
        double avg = calculateAverage(arr, size);
        int max = findMax(arr, size);
        int min = findMin(arr, size);

        cout << "합계: " << sum << endl;
        cout << "평균: " << fixed << setprecision(2) << avg << endl;
        cout << "최댓값: " << max << endl;
        cout << "최솟값: " << min << endl;
        cout << "✓ 정적 배열 (메모리 해제 불필요)" << endl << endl;
    }

    // 테스트 4: 예외 처리
    {
        cout << "📝 테스트 4: 예외 처리" << endl;

        // null 포인터
        cout << "1️⃣ null 포인터 테스트:" << endl;
        long long sum = calculateSum(nullptr, 10);
        cout << "반환값: " << sum << endl;

        // 크기 0
        cout << "\n2️⃣ 크기 0 테스트:" << endl;
        int arr[] = {1, 2, 3};
        sum = calculateSum(arr, 0);
        cout << "반환값: " << sum << endl;

        cout << "✓ 예외 처리 완료" << endl << endl;
    }

    cout << "╔═══════════════════════════════════════╗" << endl;
    cout << "║  ✅ 모든 테스트 완료!                  ║" << endl;
    cout << "╚═══════════════════════════════════════╝" << endl;

    return 0;
}

/**
 * 학습 포인트:
 *
 * 1. 포인터 전달:
 *    - 배열을 함수에 전달할 때는 포인터 사용
 *    - const int* arr: 읽기만 가능
 *
 * 2. 메모리 관리:
 *    - new[]로 할당한 배열은 delete[]로 해제
 *    - nullptr 확인으로 안전성 보장
 *
 * 3. 오류 처리:
 *    - null 포인터 확인
 *    - 범위 체크
 *
 * 심화 과제:
 * 1. calculateMedian() 함수 추가 (중앙값)
 * 2. calculateStandardDeviation() 함수 추가 (표준편차)
 * 3. sort() 함수로 배열 정렬 후 통계 재계산
 */
