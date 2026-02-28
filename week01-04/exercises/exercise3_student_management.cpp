/**
 * Exercise 3: 학생 관리 시스템 (Student Management System)
 *
 * 학습 목표:
 * - 클래스 설계 및 캡슐화
 * - STL 컨테이너 (vector, map)
 * - 동적 메모리 관리
 * - 람다 함수
 * - 알고리즘과 정렬
 *
 * 난이도: ⭐⭐⭐ 고급
 */

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <iomanip>
#include <memory>

using namespace std;

/**
 * 학생 클래스
 */
class Student {
private:
    int id;
    string name;
    double gpa;
    vector<int> scores;

public:
    // 생성자
    Student(int id, const string& name, double gpa)
        : id(id), name(name), gpa(gpa) {
        cout << "학생 생성: " << name << " (ID: " << id << ")" << endl;
    }

    // 소멸자
    ~Student() {
        cout << "학생 소멸: " << name << endl;
    }

    // Getter 함수들
    int getId() const { return id; }
    string getName() const { return name; }
    double getGPA() const { return gpa; }

    // 점수 추가
    void addScore(int score) {
        if (score >= 0 && score <= 100) {
            scores.push_back(score);
        }
    }

    // 평균 점수 계산
    double getAverageScore() const {
        if (scores.empty()) return 0.0;
        long long sum = 0;
        for (int score : scores) {
            sum += score;
        }
        return (double)sum / scores.size();
    }

    // 최고 점수 반환
    int getMaxScore() const {
        if (scores.empty()) return -1;
        return *max_element(scores.begin(), scores.end());
    }

    // 최저 점수 반환
    int getMinScore() const {
        if (scores.empty()) return -1;
        return *min_element(scores.begin(), scores.end());
    }

    // 점수 개수 반환
    int getScoreCount() const {
        return scores.size();
    }

    // 정보 출력
    void print() const {
        cout << "ID: " << setw(4) << id
             << " | 이름: " << setw(10) << name
             << " | GPA: " << fixed << setprecision(2) << gpa;

        if (!scores.empty()) {
            cout << " | 평균 점수: " << getAverageScore();
        }
        cout << endl;
    }
};

/**
 * 학생 관리 시스템 클래스
 */
class StudentManagementSystem {
private:
    vector<unique_ptr<Student>> students;
    map<int, Student*> idIndex;  // ID로 빠른 검색을 위한 인덱스

public:
    StudentManagementSystem() {}

    ~StudentManagementSystem() {
        cout << "\n시스템 종료: " << students.size() << "명의 학생 정보 삭제됨" << endl;
    }

    // 학생 추가
    bool addStudent(int id, const string& name, double gpa) {
        // 중복 ID 확인
        if (idIndex.find(id) != idIndex.end()) {
            cout << "오류: ID " << id << "는 이미 존재합니다" << endl;
            return false;
        }

        // GPA 범위 확인 (0.0 ~ 4.0)
        if (gpa < 0.0 || gpa > 4.0) {
            cout << "오류: GPA는 0.0 ~ 4.0 범위여야 합니다" << endl;
            return false;
        }

        auto student = make_unique<Student>(id, name, gpa);
        Student* ptr = student.get();
        students.push_back(move(student));
        idIndex[id] = ptr;

        return true;
    }

    // ID로 학생 검색
    Student* findStudentById(int id) {
        auto it = idIndex.find(id);
        if (it != idIndex.end()) {
            return it->second;
        }
        return nullptr;
    }

    // 이름으로 학생 검색 (모든 일치)
    vector<Student*> findStudentsByName(const string& name) {
        vector<Student*> result;
        for (auto& student : students) {
            if (student->getName() == name) {
                result.push_back(student.get());
            }
        }
        return result;
    }

    // 학생 점수 추가
    bool addScore(int id, int score) {
        Student* student = findStudentById(id);
        if (student != nullptr) {
            student->addScore(score);
            return true;
        }
        cout << "오류: ID " << id << "를 찾을 수 없습니다" << endl;
        return false;
    }

    // 전체 학생 출력
    void printAllStudents() const {
        cout << "\n=== 전체 학생 목록 ===" << endl;
        for (const auto& student : students) {
            student->print();
        }
    }

    // 학생 수 반환
    int getStudentCount() const {
        return students.size();
    }

    // GPA로 정렬 후 출력
    void printSortedByGPA(bool ascending = false) const {
        vector<Student*> sorted;
        for (const auto& student : students) {
            sorted.push_back(student.get());
        }

        if (ascending) {
            sort(sorted.begin(), sorted.end(),
                 [](const Student* a, const Student* b) {
                     return a->getGPA() < b->getGPA();
                 });
            cout << "\n=== GPA 오름차순 정렬 ===" << endl;
        } else {
            sort(sorted.begin(), sorted.end(),
                 [](const Student* a, const Student* b) {
                     return a->getGPA() > b->getGPA();
                 });
            cout << "\n=== GPA 내림차순 정렬 ===" << endl;
        }

        for (const auto& student : sorted) {
            student->print();
        }
    }

    // 평균 점수로 정렬 후 출력
    void printSortedByAverageScore() const {
        vector<Student*> sorted;
        for (const auto& student : students) {
            sorted.push_back(student.get());
        }

        sort(sorted.begin(), sorted.end(),
             [](const Student* a, const Student* b) {
                 return a->getAverageScore() > b->getAverageScore();
             });

        cout << "\n=== 평균 점수 내림차순 정렬 ===" << endl;
        for (const auto& student : sorted) {
            cout << student->getName() << ": "
                 << fixed << setprecision(2) << student->getAverageScore() << endl;
        }
    }

    // GPA 범위 내의 학생 찾기
    vector<Student*> findStudentsByGPARange(double minGPA, double maxGPA) const {
        vector<Student*> result;
        for (const auto& student : students) {
            double gpa = student->getGPA();
            if (gpa >= minGPA && gpa <= maxGPA) {
                result.push_back(student.get());
            }
        }
        return result;
    }

    // 통계 정보 출력
    void printStatistics() const {
        if (students.empty()) {
            cout << "등록된 학생이 없습니다" << endl;
            return;
        }

        cout << "\n=== 통계 정보 ===" << endl;
        cout << "총 학생 수: " << students.size() << "명" << endl;

        // GPA 통계
        double avgGPA = 0.0;
        double maxGPA = students[0]->getGPA();
        double minGPA = students[0]->getGPA();

        for (const auto& student : students) {
            avgGPA += student->getGPA();
            maxGPA = max(maxGPA, student->getGPA());
            minGPA = min(minGPA, student->getGPA());
        }
        avgGPA /= students.size();

        cout << "평균 GPA: " << fixed << setprecision(2) << avgGPA << endl;
        cout << "최고 GPA: " << maxGPA << endl;
        cout << "최저 GPA: " << minGPA << endl;

        // 점수 통계
        int totalScores = 0;
        for (const auto& student : students) {
            totalScores += student->getScoreCount();
        }
        cout << "총 시험 점수: " << totalScores << "개" << endl;
    }
};

int main() {
    cout << "╔═══════════════════════════════════════╗" << endl;
    cout << "║  Exercise 3: 학생 관리 시스템          ║" << endl;
    cout << "╚═══════════════════════════════════════╝" << endl << endl;

    // 테스트 1: 기본 학생 추가 및 출력
    {
        cout << "📝 테스트 1: 기본 학생 추가 및 출력" << endl;
        StudentManagementSystem system;

        system.addStudent(1001, "김철수", 3.8);
        system.addStudent(1002, "이영희", 3.9);
        system.addStudent(1003, "박민준", 3.5);
        system.addStudent(1004, "최지은", 4.0);
        system.addStudent(1005, "정준호", 3.2);

        system.printAllStudents();
        system.printStatistics();

        cout << endl;
    }

    // 테스트 2: 점수 추가 및 평균 계산
    {
        cout << "📝 테스트 2: 점수 추가 및 평균 계산" << endl;
        StudentManagementSystem system;

        system.addStudent(2001, "한수진", 3.7);
        system.addStudent(2002, "임동현", 3.4);

        // 점수 추가
        for (int i = 0; i < 5; i++) {
            system.addScore(2001, 85 + i * 2);  // 85, 87, 89, 91, 93
        }

        for (int i = 0; i < 4; i++) {
            system.addScore(2002, 75 + i * 3);  // 75, 78, 81, 84
        }

        cout << "\n점수 추가 후:" << endl;
        Student* s = system.findStudentById(2001);
        if (s) {
            cout << s->getName() << "의 평균 점수: "
                 << fixed << setprecision(2) << s->getAverageScore() << endl;
            cout << "최고 점수: " << s->getMaxScore()
                 << ", 최저 점수: " << s->getMinScore() << endl;
        }

        cout << endl;
    }

    // 테스트 3: 검색 및 정렬
    {
        cout << "📝 테스트 3: 검색 및 정렬" << endl;
        StudentManagementSystem system;

        system.addStudent(3001, "신민철", 3.9);
        system.addStudent(3002, "오수정", 3.5);
        system.addStudent(3003, "황준호", 3.8);
        system.addStudent(3004, "서예진", 3.2);
        system.addStudent(3005, "이준영", 3.7);

        // ID로 검색
        cout << "\n1️⃣ ID 3003으로 검색:" << endl;
        Student* student = system.findStudentById(3003);
        if (student) {
            student->print();
        }

        // GPA로 정렬
        system.printSortedByGPA(false);  // 내림차순

        // GPA 범위로 검색
        cout << "\n3️⃣ GPA 3.5 ~ 3.8 범위의 학생:" << endl;
        auto rangeStudents = system.findStudentsByGPARange(3.5, 3.8);
        for (auto s : rangeStudents) {
            cout << "  - " << s->getName() << " (GPA: " << s->getGPA() << ")" << endl;
        }

        cout << endl;
    }

    // 테스트 4: 대규모 데이터 처리
    {
        cout << "📝 테스트 4: 대규모 데이터 처리 (100명 학생)" << endl;
        StudentManagementSystem system;

        srand(42);
        for (int i = 0; i < 100; i++) {
            int id = 4000 + i;
            string name = "학생" + to_string(i + 1);
            double gpa = 2.0 + (rand() % 200) / 100.0;  // 2.0 ~ 4.0

            system.addStudent(id, name, gpa);

            // 각 학생에게 3~5개의 점수 추가
            int scoreCount = 3 + (rand() % 3);
            for (int j = 0; j < scoreCount; j++) {
                int score = 60 + (rand() % 41);  // 60 ~ 100
                system.addScore(id, score);
            }
        }

        cout << "✓ 100명의 학생 정보 추가 완료" << endl;
        system.printStatistics();

        cout << "\n상위 5명 (GPA 기준):" << endl;
        system.printSortedByGPA(false);

        cout << endl;
    }

    // 테스트 5: 중복 및 오류 처리
    {
        cout << "📝 테스트 5: 중복 및 오류 처리" << endl;
        StudentManagementSystem system;

        system.addStudent(5001, "오류테스트", 3.5);

        // 중복 ID 추가 시도
        cout << "\n1️⃣ 중복 ID 추가 시도:" << endl;
        system.addStudent(5001, "다른학생", 3.0);

        // 잘못된 GPA 시도
        cout << "\n2️⃣ 범위를 벗어난 GPA:" << endl;
        system.addStudent(5002, "학생2", 4.5);  // 범위 초과
        system.addStudent(5003, "학생3", -1.0);  // 음수

        // 존재하지 않는 학생에게 점수 추가
        cout << "\n3️⃣ 존재하지 않는 학생에게 점수 추가:" << endl;
        system.addScore(9999, 85);

        // 존재하지 않는 학생 검색
        cout << "\n4️⃣ 존재하지 않는 학생 검색:" << endl;
        Student* notFound = system.findStudentById(9999);
        if (notFound == nullptr) {
            cout << "검색 결과: 없음 ✓" << endl;
        }

        cout << endl;
    }

    cout << "╔═══════════════════════════════════════╗" << endl;
    cout << "║  ✅ 모든 테스트 완료!                  ║" << endl;
    cout << "╚═══════════════════════════════════════╝" << endl;

    return 0;
}

/**
 * 학습 포인트:
 *
 * 1. 클래스 설계:
 *    - Student: 데이터와 메서드를 캡슐화
 *    - StudentManagementSystem: 전체 시스템 관리
 *
 * 2. STL 컨테이너:
 *    - vector<unique_ptr<Student>>: 학생 목록 저장
 *    - map<int, Student*>: ID로 빠른 검색 (O(log n))
 *
 * 3. 포인터와 메모리 관리:
 *    - unique_ptr로 자동 메모리 관리
 *    - 인덱스를 통한 포인터 저장
 *
 * 4. 알고리즘:
 *    - sort()로 GPA별 정렬
 *    - max_element(), min_element()로 최대/최소 찾기
 *    - 람다 함수로 커스텀 정렬 기준
 *
 * 5. 람다 함수:
 *    - 정렬 기준을 람다로 정의
 *    - 인라인 함수로 간결한 코드
 *
 * 심화 과제:
 * 1. 학생 삭제 기능 추가
 * 2. 파일 입출력 (학생 정보 저장/로드)
 * 3. 점수 업데이트/삭제 기능
 * 4. 학적 상태 필드 추가 (재학/휴학/졸업)
 * 5. 검색 기능 강화 (부분 이름 검색)
 * 6. 성적 분포 히스토그램 출력
 */
