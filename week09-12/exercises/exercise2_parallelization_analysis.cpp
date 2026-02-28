/**
 * Exercise 2: 병렬화 분석 및 스케줄링 (Parallelization Analysis & Scheduling)
 *
 * 학습 목표:
 * - 의존성 그래프에서 병렬화 가능한 부분 감지
 * - 스케줄링 알고리즘 (List Scheduling)
 * - 성능 추정 (Makespan 계산)
 * - 로드 밸런싱
 *
 * 난이도: ⭐⭐⭐ 고급 (University Level)
 *
 * 배경: 다중 프로세서 환경에서 Task를 최적으로 배치
 * 목표: 최소 시간에 모든 Task 완료하기
 */

#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <queue>
#include <algorithm>
#include <iomanip>
#include <numeric>

using namespace std;

/**
 * Task: 작업 단위
 */
struct Task {
    int id;
    string name;
    long duration;

    Task(int id, const string& name, long duration)
        : id(id), name(name), duration(duration) {}
};

/**
 * 의존성 그래프
 */
class DependencyGraph {
private:
    map<int, shared_ptr<Task>> tasks;
    map<int, vector<int>> adjacency;  // id → [의존 Task들]
    map<int, vector<int>> reverseAdj;

public:
    void addTask(int id, const string& name, long duration) {
        tasks[id] = make_shared<Task>(id, name, duration);
    }

    void addEdge(int from, int to) {
        adjacency[to].push_back(from);
        reverseAdj[from].push_back(to);
    }

    vector<int> getDependencies(int id) const {
        auto it = adjacency.find(id);
        return it != adjacency.end() ? it->second : vector<int>();
    }

    vector<int> getDependents(int id) const {
        auto it = reverseAdj.find(id);
        return it != reverseAdj.end() ? it->second : vector<int>();
    }

    shared_ptr<Task> getTask(int id) const {
        auto it = tasks.find(id);
        return it != tasks.end() ? it->second : nullptr;
    }

    int getTaskCount() const { return tasks.size(); }

    const map<int, shared_ptr<Task>>& getTasks() const {
        return tasks;
    }

    /**
     * 위상 정렬
     */
    vector<int> topologicalSort() {
        map<int, int> inDegree;
        for (const auto& [id, _] : tasks) {
            inDegree[id] = 0;
        }

        for (const auto& [to, froms] : adjacency) {
            inDegree[to] = froms.size();
        }

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

            for (int dependent : getDependents(current)) {
                tempInDegree[dependent]--;
                if (tempInDegree[dependent] == 0) {
                    q.push(dependent);
                }
            }
        }

        return result;
    }

    /**
     * 각 Task의 최이른 시작 시간 (EST: Earliest Start Time) 계산
     */
    map<int, long> calculateEST() {
        auto topoOrder = topologicalSort();
        map<int, long> est;

        for (int id : topoOrder) {
            long maxDepTime = 0;
            for (int depId : getDependencies(id)) {
                maxDepTime = max(maxDepTime,
                    est[depId] + getTask(depId)->duration);
            }
            est[id] = maxDepTime;
        }

        return est;
    }

    /**
     * 각 Task의 최늦 완료 시간 (LCT: Latest Completion Time) 계산
     */
    map<int, long> calculateLCT() {
        auto topoOrder = topologicalSort();
        reverse(topoOrder.begin(), topoOrder.end());

        // 마지막 Task들의 LCT는 EST + Duration
        long maxFinishTime = 0;
        for (int id : topoOrder) {
            auto est = calculateEST();
            maxFinishTime = max(maxFinishTime,
                est[id] + getTask(id)->duration);
        }

        map<int, long> lct;
        for (const auto& [id, _] : tasks) {
            lct[id] = maxFinishTime;
        }

        for (int id : topoOrder) {
            long minDepTime = maxFinishTime;
            for (int depId : getDependents(id)) {
                minDepTime = min(minDepTime, lct[depId]);
            }
            lct[id] = minDepTime - getTask(id)->duration;
        }

        return lct;
    }

    /**
     * Critical Task 식별 (여유 시간 = 0)
     */
    vector<int> getCriticalTasks() {
        auto est = calculateEST();
        auto lct = calculateLCT();
        vector<int> critical;

        for (const auto& [id, _] : tasks) {
            long slack = lct[id] - est[id];
            if (slack <= 0) {
                critical.push_back(id);
            }
        }

        return critical;
    }
};

/**
 * List Scheduling: 우선도 기반 스케줄링
 */
class ListScheduler {
private:
    DependencyGraph graph;
    int processorCount;

public:
    ListScheduler(const DependencyGraph& g, int processors)
        : graph(g), processorCount(processors) {}

    /**
     * 스케줄 생성
     * 반환: {taskId → (processor, startTime, endTime)}
     */
    map<int, tuple<int, long, long>> schedule() {
        map<int, long> processorTime(processorCount, 0);  // 각 프로세서의 현재 시간
        map<int, tuple<int, long, long>> schedule;  // taskId → (proc, start, end)
        map<int, bool> scheduled;

        auto topoOrder = graph.topologicalSort();

        for (int taskId : topoOrder) {
            // 이 Task의 모든 의존 Task가 스케줄되었는지 확인
            auto deps = graph.getDependencies(taskId);
            long earliestStart = 0;

            for (int depId : deps) {
                auto [depProc, depStart, depEnd] = schedule[depId];
                earliestStart = max(earliestStart, depEnd);
            }

            // 가장 빨리 비는 프로세서 찾기
            int bestProc = 0;
            long bestTime = processorTime[0];
            for (int i = 1; i < processorCount; i++) {
                if (processorTime[i] < bestTime) {
                    bestTime = processorTime[i];
                    bestProc = i;
                }
            }

            long startTime = max(bestTime, earliestStart);
            long endTime = startTime + graph.getTask(taskId)->duration;

            schedule[taskId] = make_tuple(bestProc, startTime, endTime);
            processorTime[bestProc] = endTime;
        }

        return schedule;
    }

    /**
     * Makespan 계산 (모든 Task 완료 시간)
     */
    long calculateMakespan(const map<int, tuple<int, long, long>>& sched) {
        long maxTime = 0;
        for (const auto& [taskId, timing] : sched) {
            auto [proc, start, end] = timing;
            maxTime = max(maxTime, end);
        }
        return maxTime;
    }

    /**
     * 로드 밸런싱 지수 계산
     * - 1.0이 완벽한 밸런싱 (모든 프로세서 동일 시간)
     * - 값이 작을수록 불균형
     */
    double calculateLoadBalance(const map<int, tuple<int, long, long>>& sched) {
        vector<long> procTime(processorCount, 0);

        for (const auto& [taskId, timing] : sched) {
            auto [proc, start, end] = timing;
            procTime[proc] = max(procTime[proc], end);
        }

        long avgTime = accumulate(procTime.begin(), procTime.end(), 0L)
                       / processorCount;
        long maxTime = *max_element(procTime.begin(), procTime.end());

        if (maxTime == 0) return 1.0;
        return (double)avgTime / maxTime;
    }

    /**
     * 병렬 효율성 (Parallel Efficiency)
     * = 순차 실행 시간 / (병렬 실행 시간 * 프로세서 수)
     */
    double calculateParallelEfficiency(
        const map<int, tuple<int, long, long>>& sched) {
        long sequentialTime = 0;
        for (const auto& [taskId, task] : graph.getTasks()) {
            sequentialTime += task->duration;
        }

        long parallelTime = calculateMakespan(sched);
        double speedup = (double)sequentialTime / parallelTime;
        double efficiency = speedup / processorCount;

        return min(efficiency, 1.0);  // 최대 1.0
    }
};

/**
 * 스케줄 시각화
 */
void visualizeSchedule(const map<int, tuple<int, long, long>>& sched,
                       int processorCount) {
    cout << "\n=== 스케줄 시각화 ===" << endl;

    // 각 프로세서별 Task 시간 찾기
    vector<long> procMaxTime(processorCount, 0);
    for (const auto& [taskId, timing] : sched) {
        auto [proc, start, end] = timing;
        procMaxTime[proc] = max(procMaxTime[proc], end);
    }

    long totalTime = *max_element(procMaxTime.begin(), procMaxTime.end());

    // Gantt 차트
    for (int proc = 0; proc < processorCount; proc++) {
        cout << "Proc " << proc << ": ";

        // Task를 시간순으로 정렬
        vector<pair<long, int>> taskTimes;
        for (const auto& [taskId, timing] : sched) {
            auto [p, start, end] = timing;
            if (p == proc) {
                taskTimes.push_back({start, taskId});
            }
        }
        sort(taskTimes.begin(), taskTimes.end());

        long currentTime = 0;
        for (const auto& [start, taskId] : taskTimes) {
            // 빈 공간 표시
            for (long i = currentTime; i < start; i++) {
                cout << ".";
            }
            // Task 표시
            auto [p, s, e] = sched.at(taskId);
            for (long i = s; i < e; i++) {
                cout << "T" << taskId;
            }
            currentTime = e;
        }

        // 마지막 빈 공간
        for (long i = currentTime; i < totalTime; i++) {
            cout << ".";
        }
        cout << endl;
    }

    cout << "총 시간: " << totalTime << "ms" << endl;
}

// ============================================================================
// 테스트
// ============================================================================

/**
 * 테스트 1: 기본 스케줄링
 */
void testBasicScheduling() {
    cout << "\n=== 테스트 1: 기본 스케줄링 (2 프로세서) ===" << endl;

    DependencyGraph graph;

    // Task 생성
    graph.addTask(1, "A", 10);
    graph.addTask(2, "B", 20);
    graph.addTask(3, "C", 15);
    graph.addTask(4, "D", 25);

    // 의존성
    graph.addEdge(1, 2);
    graph.addEdge(1, 3);
    graph.addEdge(2, 4);
    graph.addEdge(3, 4);

    ListScheduler scheduler(graph, 2);
    auto sched = scheduler.schedule();

    cout << "스케줄 결과:" << endl;
    for (const auto& [taskId, timing] : sched) {
        auto [proc, start, end] = timing;
        cout << "  Task " << taskId << ": Proc " << proc
             << " [" << start << "-" << end << "]" << endl;
    }

    long makespan = scheduler.calculateMakespan(sched);
    cout << "\nMakespan: " << makespan << "ms" << endl;

    double lb = scheduler.calculateLoadBalance(sched);
    cout << "로드 밸런싱: " << fixed << setprecision(2) << lb << endl;

    double eff = scheduler.calculateParallelEfficiency(sched);
    cout << "병렬 효율: " << eff * 100 << "%" << endl;

    visualizeSchedule(sched, 2);
}

/**
 * 테스트 2: Critical Path 분석
 */
void testCriticalPath() {
    cout << "\n=== 테스트 2: Critical Path 분석 ===" << endl;

    DependencyGraph graph;

    for (int i = 1; i <= 5; i++) {
        graph.addTask(i, "T" + to_string(i), 10 * i);
    }

    graph.addEdge(1, 2);
    graph.addEdge(1, 3);
    graph.addEdge(2, 4);
    graph.addEdge(3, 4);
    graph.addEdge(4, 5);

    auto critical = graph.getCriticalTasks();

    cout << "Critical Task들:" << endl;
    for (int id : critical) {
        auto task = graph.getTask(id);
        cout << "  Task " << id << " (" << task->name << ") - "
             << task->duration << "ms" << endl;
    }

    cout << "\n✓ Critical Path = 최우선으로 최적화해야 할 경로" << endl;
}

/**
 * 테스트 3: 프로세서 개수에 따른 성능 비교
 */
void testScalability() {
    cout << "\n=== 테스트 3: 프로세서 개수에 따른 성능 ===" << endl;

    DependencyGraph graph;

    for (int i = 1; i <= 8; i++) {
        graph.addTask(i, "Task" + to_string(i), 10);
    }

    // Chain 의존성
    for (int i = 1; i < 8; i++) {
        graph.addEdge(i, i + 1);
    }

    cout << "Task 수: 8, 순차 시간: " << 80 << "ms" << endl;
    cout << endl;

    for (int numProc = 1; numProc <= 8; numProc++) {
        ListScheduler scheduler(graph, numProc);
        auto sched = scheduler.schedule();

        long makespan = scheduler.calculateMakespan(sched);
        double speedup = 80.0 / makespan;
        double efficiency = speedup / numProc;

        cout << "프로세서 " << numProc << ": "
             << "Makespan=" << setw(3) << makespan << "ms "
             << "Speedup=" << fixed << setprecision(2) << speedup << "x "
             << "효율=" << efficiency * 100 << "%" << endl;
    }
}

/**
 * 테스트 4: 복잡한 그래프
 */
void testComplexGraph() {
    cout << "\n=== 테스트 4: 복잡한 그래프 (10 Task, 3 프로세서) ===" << endl;

    DependencyGraph graph;

    for (int i = 1; i <= 10; i++) {
        graph.addTask(i, "T" + to_string(i), 5 + (i % 3) * 10);
    }

    // 복잡한 의존성
    graph.addEdge(1, 2);
    graph.addEdge(1, 3);
    graph.addEdge(2, 4);
    graph.addEdge(3, 4);
    graph.addEdge(3, 5);
    graph.addEdge(4, 6);
    graph.addEdge(5, 6);
    graph.addEdge(6, 7);
    graph.addEdge(7, 8);
    graph.addEdge(7, 9);
    graph.addEdge(8, 10);
    graph.addEdge(9, 10);

    ListScheduler scheduler(graph, 3);
    auto sched = scheduler.schedule();

    long makespan = scheduler.calculateMakespan(sched);
    double lb = scheduler.calculateLoadBalance(sched);
    double eff = scheduler.calculateParallelEfficiency(sched);

    cout << "Makespan: " << makespan << "ms" << endl;
    cout << "로드 밸런싱: " << fixed << setprecision(2) << lb << endl;
    cout << "병렬 효율: " << eff * 100 << "%" << endl;

    visualizeSchedule(sched, 3);

    cout << "\n✓ 복잡한 그래프 분석 완료" << endl;
}

int main() {
    cout << "╔═══════════════════════════════════════╗" << endl;
    cout << "║  Exercise 2: 병렬화 분석 & 스케줄링   ║" << endl;
    cout << "╚═══════════════════════════════════════╝" << endl;

    try {
        testBasicScheduling();
        testCriticalPath();
        testScalability();
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
 * 1. EST/LCT 계산:
 *    - EST (Earliest Start Time): 가장 빨리 시작할 수 있는 시간
 *    - LCT (Latest Completion Time): 가장 늦게 끝낼 수 있는 시간
 *    - Slack = LCT - EST: 여유 시간
 *
 * 2. List Scheduling:
 *    - 탐욕 알고리즘 (Greedy)
 *    - 위상 순서로 Task 배정
 *    - 의존성 제약 만족
 *
 * 3. 성능 지표:
 *    - Makespan: 전체 완료 시간
 *    - Speedup: 순차 대비 병렬 속도
 *    - 효율성: Speedup / 프로세서 수
 *    - 로드 밸런싱: 프로세서 간 시간 균형
 *
 * 4. Critical Path:
 *    - 여유 시간이 0인 Task들의 경로
 *    - 이 경로를 단축해야 전체 시간 단축
 *
 * 5. 스케일러빌리티:
 *    - 프로세서 증가에 따른 성능 향상
 *    - 의존성이 많을수록 병렬화 어려움
 *
 * 심화 과제:
 * 1. 우선도 기반 스케줄링 (Priority Scheduling)
 * 2. 동적 스케줄링 (런타임 Task 할당)
 * 3. 통신 비용 모델링
 * 4. 메모리 제약 고려
 * 5. 이질적 프로세서 (속도 다름)
 */
