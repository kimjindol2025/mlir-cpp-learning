/**
 * Exercise 1: Task Graph 및 DAG (Directed Acyclic Graph)
 *
 * 학습 목표:
 * - Task Graph 데이터 구조 설계
 * - 의존성 관계 표현
 * - 그래프 검증 (순환 참조 감지)
 * - 기본 그래프 연산
 *
 * 난이도: ⭐⭐⭐ 고급 (University Level 입문)
 *
 * 배경: Python의 Task Graph를 C++로 포팅하는 첫 단계
 * 목표: 병렬 계산 그래프를 표현하고 분석
 */

#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <queue>
#include <memory>
#include <algorithm>
#include <iomanip>

using namespace std;

/**
 * Task: 계산 그래프의 개별 작업
 */
class Task {
private:
    int id;
    string name;
    long duration;  // 실행 시간 (ms)
    int priority;   // 우선도

public:
    Task(int id, const string& name, long duration, int priority = 0)
        : id(id), name(name), duration(duration), priority(priority) {}

    // Getter
    int getId() const { return id; }
    string getName() const { return name; }
    long getDuration() const { return duration; }
    int getPriority() const { return priority; }

    // Setter
    void setDuration(long d) { duration = d; }
    void setPriority(int p) { priority = p; }

    void print() const {
        cout << "Task(" << setw(2) << id << ") " << setw(12) << name
             << " duration=" << setw(4) << duration << "ms"
             << " priority=" << priority << endl;
    }
};

/**
 * TaskGraph: 작업 그래프
 *
 * 구조:
 * - 노드: Task
 * - 간선: 의존성 (Task A → Task B: A가 완료되어야 B 시작)
 */
class TaskGraph {
private:
    map<int, shared_ptr<Task>> tasks;
    map<int, vector<int>> adjacencyList;  // id → [의존 task id들]
    map<int, vector<int>> reverseAdjList;  // 역방향 (의존처)

public:
    TaskGraph() {}

    /**
     * Task 추가
     */
    bool addTask(int id, const string& name, long duration) {
        if (tasks.find(id) != tasks.end()) {
            cout << "오류: Task " << id << "은 이미 존재합니다" << endl;
            return false;
        }

        tasks[id] = make_shared<Task>(id, name, duration);
        return true;
    }

    /**
     * 의존성 추가 (from → to: from이 완료되어야 to 시작)
     */
    bool addDependency(int from, int to) {
        // 두 Task 모두 존재하는지 확인
        if (tasks.find(from) == tasks.end()) {
            cout << "오류: Task " << from << "이 존재하지 않습니다" << endl;
            return false;
        }
        if (tasks.find(to) == tasks.end()) {
            cout << "오류: Task " << to << "이 존재하지 않습니다" << endl;
            return false;
        }

        // 동일한 의존성 중복 확인
        auto& deps = adjacencyList[to];
        if (find(deps.begin(), deps.end(), from) != deps.end()) {
            cout << "경고: 의존성 " << from << " → " << to
                 << "은 이미 존재합니다" << endl;
            return false;
        }

        adjacencyList[to].push_back(from);
        reverseAdjList[from].push_back(to);

        return true;
    }

    /**
     * 순환 참조 감지 (DFS)
     */
    bool hasCycle() {
        map<int, int> visited;  // 0: 미방문, 1: 방문 중, 2: 완료

        for (const auto& [id, _] : tasks) {
            if (visited[id] == 0) {
                if (hasCycleDFS(id, visited)) {
                    return true;
                }
            }
        }

        return false;
    }

    /**
     * 모든 Task 출력
     */
    void printAllTasks() const {
        cout << "\n=== Task 목록 ===" << endl;
        for (const auto& [id, task] : tasks) {
            task->print();
        }
    }

    /**
     * 모든 의존성 출력
     */
    void printDependencies() const {
        cout << "\n=== 의존성 관계 ===" << endl;
        for (const auto& [to, froms] : adjacencyList) {
            for (int from : froms) {
                cout << "  Task " << from << " → Task " << to << endl;
            }
        }
    }

    /**
     * Task 개수
     */
    int getTaskCount() const { return tasks.size(); }

    /**
     * 특정 Task의 직접 의존 Task들 반환
     */
    vector<int> getDependencies(int taskId) const {
        auto it = adjacencyList.find(taskId);
        if (it != adjacencyList.end()) {
            return it->second;
        }
        return {};
    }

    /**
     * 특정 Task에 의존하는 Task들 반환
     */
    vector<int> getDependents(int taskId) const {
        auto it = reverseAdjList.find(taskId);
        if (it != reverseAdjList.end()) {
            return it->second;
        }
        return {};
    }

    /**
     * Critical Path 길이 계산
     * - 가장 오래 걸리는 경로
     */
    long getCriticalPathLength() {
        // 위상 정렬 순서로 계산
        auto topoOrder = topologicalSort();
        if (topoOrder.empty()) {
            return 0;
        }

        // 각 Task의 최대 완료 시간
        map<int, long> finishTime;

        for (int id : topoOrder) {
            long maxDepTime = 0;

            // 의존 Task들의 완료 시간 중 최대값
            for (int depId : getDependencies(id)) {
                maxDepTime = max(maxDepTime, finishTime[depId]);
            }

            finishTime[id] = maxDepTime + tasks[id]->getDuration();
        }

        // 모든 Task의 완료 시간 중 최대값
        long criticalPath = 0;
        for (const auto& [id, time] : finishTime) {
            criticalPath = max(criticalPath, time);
        }

        return criticalPath;
    }

    /**
     * 위상 정렬 (Topological Sort) - Kahn의 알고리즘
     */
    vector<int> topologicalSort() {
        // 진입 차수(in-degree) 계산
        map<int, int> inDegree;
        for (const auto& [id, _] : tasks) {
            inDegree[id] = 0;
        }

        for (const auto& [to, froms] : adjacencyList) {
            inDegree[to] = froms.size();
        }

        // 진입 차수가 0인 노드부터 시작
        queue<int> q;
        for (const auto& [id, degree] : inDegree) {
            if (degree == 0) {
                q.push(id);
            }
        }

        vector<int> result;
        map<int, int> tempInDegree = inDegree;

        while (!q.empty()) {
            int current = q.front();
            q.pop();
            result.push_back(current);

            // 현재 노드에 의존하는 노드들
            for (int dependent : getDependents(current)) {
                tempInDegree[dependent]--;
                if (tempInDegree[dependent] == 0) {
                    q.push(dependent);
                }
            }
        }

        // 모든 노드가 처리되었는지 확인 (순환 참조 감지)
        if (result.size() != tasks.size()) {
            cout << "오류: 순환 참조 감지!" << endl;
            return {};
        }

        return result;
    }

    /**
     * 각 Task의 레벨 계산 (병렬화 단위)
     * - 같은 레벨의 Task들은 병렬 실행 가능
     */
    map<int, int> calculateLevels() {
        map<int, int> levels;

        // 레벨 계산 (DFS)
        function<int(int)> calcLevel = [&](int id) -> int {
            if (levels.find(id) != levels.end()) {
                return levels[id];
            }

            int maxDepLevel = -1;
            for (int depId : getDependencies(id)) {
                maxDepLevel = max(maxDepLevel, calcLevel(depId));
            }

            return levels[id] = maxDepLevel + 1;
        };

        for (const auto& [id, _] : tasks) {
            calcLevel(id);
        }

        return levels;
    }

    /**
     * 병렬화 가능 레벨별 Task 그룹화
     */
    vector<vector<int>> getParallelLevels() {
        auto levels = calculateLevels();

        // 레벨별로 Task 그룹화
        map<int, vector<int>> levelGroups;
        for (const auto& [id, level] : levels) {
            levelGroups[level].push_back(id);
        }

        // 결과 구성
        vector<vector<int>> result;
        for (const auto& [level, tasks] : levelGroups) {
            result.push_back(tasks);
        }

        return result;
    }

    /**
     * 그래프 통계
     */
    void printStatistics() const {
        cout << "\n=== 그래프 통계 ===" << endl;
        cout << "Task 수: " << tasks.size() << endl;

        int edgeCount = 0;
        for (const auto& [to, froms] : adjacencyList) {
            edgeCount += froms.size();
        }
        cout << "의존성 수: " << edgeCount << endl;

        // 진입/진출 차수 통계
        int maxInDegree = 0, maxOutDegree = 0;
        for (const auto& [id, _] : tasks) {
            int inDeg = getDependencies(id).size();
            int outDeg = getDependents(id).size();
            maxInDegree = max(maxInDegree, inDeg);
            maxOutDegree = max(maxOutDegree, outDeg);
        }
        cout << "최대 진입 차수: " << maxInDegree << endl;
        cout << "최대 진출 차수: " << maxOutDegree << endl;
    }

private:
    /**
     * DFS로 순환 참조 감지
     */
    bool hasCycleDFS(int node, map<int, int>& visited) {
        visited[node] = 1;  // 방문 중

        for (int dependent : getDependents(node)) {
            if (visited[dependent] == 1) {
                cout << "순환 참조 발견: ... → " << node << " → "
                     << dependent << " → ..." << endl;
                return true;
            }
            if (visited[dependent] == 0) {
                if (hasCycleDFS(dependent, visited)) {
                    return true;
                }
            }
        }

        visited[node] = 2;  // 완료
        return false;
    }
};

// ============================================================================
// 테스트 코드
// ============================================================================

/**
 * 테스트 1: 기본 Task Graph 생성
 */
void testBasicGraph() {
    cout << "\n=== 테스트 1: 기본 Task Graph ===" << endl;

    TaskGraph graph;

    // Task 추가
    graph.addTask(1, "A", 10);
    graph.addTask(2, "B", 20);
    graph.addTask(3, "C", 15);
    graph.addTask(4, "D", 25);

    // 의존성: A → B, A → C, B → D, C → D
    graph.addDependency(1, 2);  // A → B
    graph.addDependency(1, 3);  // A → C
    graph.addDependency(2, 4);  // B → D
    graph.addDependency(3, 4);  // C → D

    graph.printAllTasks();
    graph.printDependencies();
    graph.printStatistics();

    cout << "\n✓ Task Graph 생성 완료" << endl;
}

/**
 * 테스트 2: 위상 정렬
 */
void testTopologicalSort() {
    cout << "\n=== 테스트 2: 위상 정렬 ===" << endl;

    TaskGraph graph;

    for (int i = 1; i <= 6; i++) {
        graph.addTask(i, "Task" + to_string(i), 10 * i);
    }

    graph.addDependency(1, 2);
    graph.addDependency(1, 3);
    graph.addDependency(2, 4);
    graph.addDependency(3, 4);
    graph.addDependency(4, 5);
    graph.addDependency(4, 6);

    auto topoOrder = graph.topologicalSort();

    cout << "위상 정렬 순서: ";
    for (int id : topoOrder) {
        cout << id << " ";
    }
    cout << endl;

    cout << "✓ 위상 정렬 완료" << endl;
}

/**
 * 테스트 3: Critical Path 계산
 */
void testCriticalPath() {
    cout << "\n=== 테스트 3: Critical Path 분석 ===" << endl;

    TaskGraph graph;

    graph.addTask(1, "Start", 0);
    graph.addTask(2, "ProcessA", 50);
    graph.addTask(3, "ProcessB", 30);
    graph.addTask(4, "Merge", 20);
    graph.addTask(5, "End", 0);

    graph.addDependency(1, 2);
    graph.addDependency(1, 3);
    graph.addDependency(2, 4);
    graph.addDependency(3, 4);
    graph.addDependency(4, 5);

    long criticalPath = graph.getCriticalPathLength();

    cout << "Critical Path 길이: " << criticalPath << "ms" << endl;
    cout << "✓ 최장 경로: Start → ProcessA(50) → Merge(20) → End(0) = 70ms" << endl;
}

/**
 * 테스트 4: 병렬화 레벨 분석
 */
void testParallelLevels() {
    cout << "\n=== 테스트 4: 병렬화 레벨 분석 ===" << endl;

    TaskGraph graph;

    for (int i = 1; i <= 7; i++) {
        graph.addTask(i, "T" + to_string(i), 10);
    }

    // 레벨별 의존성
    graph.addDependency(1, 2);  // Level 0 → Level 1
    graph.addDependency(1, 3);
    graph.addDependency(2, 4);  // Level 1 → Level 2
    graph.addDependency(3, 4);
    graph.addDependency(3, 5);
    graph.addDependency(4, 6);  // Level 2 → Level 3
    graph.addDependency(5, 6);
    graph.addDependency(6, 7);  // Level 3 → Level 4

    auto parallelLevels = graph.getParallelLevels();

    cout << "병렬화 가능 레벨:" << endl;
    for (size_t level = 0; level < parallelLevels.size(); level++) {
        cout << "  레벨 " << level << ": Task [ ";
        for (int id : parallelLevels[level]) {
            cout << id << " ";
        }
        cout << "] (병렬 실행 가능)" << endl;
    }

    cout << "✓ 총 " << parallelLevels.size() << "개 레벨 분석 완료" << endl;
}

/**
 * 테스트 5: 순환 참조 감지
 */
void testCycleDetection() {
    cout << "\n=== 테스트 5: 순환 참조 감지 ===" << endl;

    TaskGraph graph;

    graph.addTask(1, "A", 10);
    graph.addTask(2, "B", 10);
    graph.addTask(3, "C", 10);

    // 정상 의존성
    graph.addDependency(1, 2);
    graph.addDependency(2, 3);

    cout << "정상 그래프 (1→2→3):" << endl;
    if (!graph.hasCycle()) {
        cout << "✓ 순환 참조 없음" << endl;
    }

    // 순환 참조 생성
    TaskGraph cycleGraph;
    cycleGraph.addTask(1, "X", 10);
    cycleGraph.addTask(2, "Y", 10);
    cycleGraph.addTask(3, "Z", 10);

    cycleGraph.addDependency(1, 2);
    cycleGraph.addDependency(2, 3);
    cycleGraph.addDependency(3, 1);  // 순환!

    cout << "\n순환 참조 그래프 (1→2→3→1):" << endl;
    if (cycleGraph.hasCycle()) {
        cout << "✓ 순환 참조 감지됨!" << endl;
    }
}

/**
 * 테스트 6: 복잡한 그래프
 */
void testComplexGraph() {
    cout << "\n=== 테스트 6: 복잡한 그래프 (10개 Task) ===" << endl;

    TaskGraph graph;

    for (int i = 1; i <= 10; i++) {
        graph.addTask(i, "Task" + to_string(i), 5 + i * 3);
    }

    // 다단계 의존성
    graph.addDependency(1, 2);
    graph.addDependency(1, 3);
    graph.addDependency(2, 4);
    graph.addDependency(3, 4);
    graph.addDependency(3, 5);
    graph.addDependency(4, 6);
    graph.addDependency(5, 6);
    graph.addDependency(6, 7);
    graph.addDependency(7, 8);
    graph.addDependency(7, 9);
    graph.addDependency(8, 10);
    graph.addDependency(9, 10);

    graph.printStatistics();

    auto topoOrder = graph.topologicalSort();
    cout << "\n위상 정렬: ";
    for (int id : topoOrder) {
        cout << id << " ";
    }
    cout << endl;

    long criticalPath = graph.getCriticalPathLength();
    cout << "\nCritical Path: " << criticalPath << "ms" << endl;

    auto parallelLevels = graph.getParallelLevels();
    cout << "병렬화 레벨: " << parallelLevels.size() << "개" << endl;

    cout << "✓ 복잡한 그래프 분석 완료" << endl;
}

int main() {
    cout << "╔═══════════════════════════════════════╗" << endl;
    cout << "║  Exercise 1: Task Graph & DAG         ║" << endl;
    cout << "╚═══════════════════════════════════════╝" << endl;

    try {
        testBasicGraph();
        testTopologicalSort();
        testCriticalPath();
        testParallelLevels();
        testCycleDetection();
        testComplexGraph();

        cout << "\n╔═══════════════════════════════════════╗" << endl;
        cout << "║  ✅ 모든 테스트 완료!                  ║" << endl;
        cout << "╚═══════════════════════════════════════╝" << endl;

        return 0;
    } catch (const exception& e) {
        cerr << "오류: " << e.what() << endl;
        return 1;
    }
}

/**
 * 학습 포인트:
 *
 * 1. DAG (Directed Acyclic Graph):
 *    - 노드: Task
 *    - 간선: 의존성 (방향성, 비순환)
 *
 * 2. 인접 리스트 표현:
 *    - 메모리 효율적
 *    - O(V + E) 순회 가능
 *
 * 3. 위상 정렬 (Kahn의 알고리즘):
 *    - O(V + E) 시간 복잡도
 *    - 진입 차수(in-degree) 활용
 *    - 순환 참조 감지 가능
 *
 * 4. Critical Path:
 *    - 가장 오래 걸리는 경로
 *    - 병렬화 시 최소 소요 시간
 *
 * 5. 병렬화 레벨:
 *    - 같은 레벨 Task는 병렬 실행 가능
 *    - Task 할당(scheduling) 기반
 *
 * 6. 순환 참조 감지:
 *    - DFS 기반
 *    - visited 상태: 0(미방문), 1(방문중), 2(완료)
 *
 * 심화 과제:
 * 1. 무게 추가 (Task 크기, 데이터 전송 크기)
 * 2. 스케줄링 알고리즘 (List Scheduling)
 * 3. 로드 밸런싱
 * 4. 에지 라벨링 (데이터 의존성 크기)
 * 5. 병렬화 비용 모델
 */
