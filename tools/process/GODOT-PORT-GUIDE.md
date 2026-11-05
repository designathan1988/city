# Roteiro de execução: Godot 4.7 + C++ como motor oficial do Roadcraft

Execute este roteiro **do início ao fim, na ordem, sem parar**. Cada passo diz onde está o código, o que fazer e
como. As decisões já foram tomadas: não reabra, não investigue alternativas, não meça nem teste entre passos.
Ao terminar todos os blocos, compile (bloco G) e entregue o relatório.

## 0. Caminhos e convenções

- `G` = projeto Godot (onde você trabalha): `C:\Users\Jonathan\Documents\Projects\city\roadcraft`
  (se não existir, pergunte o caminho uma vez e siga).
- `N` = roadcraft-novo, **somente leitura**: `C:\Projects\roadcraft-novo`.
  Os caminhos de `N` abaixo são da versão do GitHub. Se um arquivo não estiver no caminho indicado, ache-o pelo nome
  (`dir /s /b nome.ts`) e siga.
- Os arquivos C++ ficam em `G/native/src/...`, o GDScript em `G/project/...` e os dados em `G/project/data/...`.
- Código em inglês (identificadores, comentários). Todo arquivo novo começa com um comentário de cabeçalho dizendo
  a responsabilidade dele (o gate exige).
- Limites dos gates: `.cpp` ≤ 350 linhas, `.h` ≤ 250, `.gd` ≤ 250, função ≤ 90 linhas. Se passar, divida por
  responsabilidade.
- Sem números mágicos em `native/src`, `project/game`, `project/ui`: crie uma constante nomeada no arquivo dono
  ou uma chave em `project/data/**` (com schema em `tools/gates/schemas/**` e campo em `CITY_TUNING_FIELDS`
  quando for tuning).
- Um commit por bloco (A, B, C, D, E, F, G), com mensagem em português descrevendo o que mudou.

### Tradução TypeScript → C++ (para os portes do bloco E)
| TypeScript (N) | C++ (G) |
|---|---|
| `number` de posição/comprimento de via | `double` (como `RoadSegment`); posição de render e de agente: `float` |
| `Float32Array`/`Int32Array`/`Uint8Array` | `std::vector<float/int32_t/uint8_t>` |
| `Table` SoA de `core/table.ts` | `std::vector<Struct>` contíguo (ver decisão B0) |
| `Map<number, X>` / `Set<number>` | `std::unordered_map` / `std::unordered_set`, ou vetor indexado por id quando o id é denso |
| `class` com estado | `struct` de dados + funções livres no `namespace city` (padrão do projeto) |
| closures / callbacks | lambdas (`template <typename F>` como em `SpatialHash::query`) |
| `Math.hypot`, `Math.atan2` | `std::hypot`, `std::atan2` |
| constantes no topo do .ts | `constexpr` no .cpp (se for ajuste de jogo: chave em `data/catalog/*.json`) |
| texto para o jogador | chave em `project/data/i18n/strings.csv` + `tr("CHAVE")` |
| DOM/HTML/CSS da interface | GDScript em `project/ui/**` com botões/abas vindos de `data/ui/*.json` |
| three.js / TSL | não portar: usar RenderingServer, MultiMesh, visibility range e `.gdshader` |

---

## Bloco A — Preparar

1. Em `G`: se não houver `.git`, rode `git init`, `git add -A` e `git commit -m "Base antes do porte"`.
2. Se `G/native/godot-cpp` não existir: `git clone https://github.com/godotengine/godot-cpp native/godot-cpp`.
   O `CMakeLists.txt` já fixa a API 4.7 (`GODOTCPP_API_VERSION`).
3. Em `G/tools/build/build.sh`, troque `JOBS="${JOBS:-2}"` por `JOBS="${JOBS:-$(nproc)}"`. Troque os alvos
   quebrados: em `e2e()` use `res://tests/core/road_commands_test.tscn`, `res://tests/core/save_undo_test.tscn` e
   `res://tests/core/camera_input_test.tscn` (um `godot --headless` por cena); em `capture()` use
   `res://tests/visual/capture.tscn`. Apague os `.uid` órfãos `project/tests/visual/capture_game.gd.uid` e
   `game_driver.gd.uid`.
4. Em `G/project/game/flow/game_flow.gd`: hoje ele aponta para `game/game.tscn`, `ui/menus/main_menu.tscn`,
   `CityStore.new_city` e `CityStore.load_from`, que não existem. O menu é criado no bloco E3. Até lá, deixe o
   `GameFlow` com os estados e transições e troque `GAME_SCENE` por `res://game/play.tscn`.

---

## Bloco B — Threads, travas e fluxo de dados (a base de tudo)

### B0. Decisões (não reabrir)
- **Não converta `Vehicle`/`Pedestrian` para SoA.** A lição "nenhum objeto por entidade" do novo vale para objetos
  JS no heap (coletor de lixo). Em C++, `std::vector<Vehicle>` já é contíguo e sem coletor. O que se corrige é a
  **alocação por veículo**: a rota (`std::vector<int> route`) vai para uma arena (C1).
- **As rotas ficam na thread da simulação**, com orçamento por passo, como no novo (o `Router` dele roda dentro do
  worker da simulação). Apague `graph_mutex` e o comentário que fala de "pathfinding threads".
- A trava única vira **duas travas**: `map_mutex` (`std::shared_mutex`), que protege terreno, vias, lotes,
  prédios, crescimento, histórico, dinheiro e relógio, e `traffic_mutex` (`std::mutex`), que protege
  `TrafficState`. O render nunca pega `traffic_mutex`: lê as posições pelo buffer triplo (B2).

### B1. Travas novas — `native/src/sim/state/city_state.h`
1. Troque `std::mutex world_mutex; std::shared_mutex graph_mutex;` por:
   ```cpp
   // map_mutex: shared by readers (simulation traffic step, queries, mesh jobs); exclusive for commands, growth,
   // clock, load. traffic_mutex: held by the simulation thread while it steps traffic (probes take it too).
   std::shared_mutex map_mutex;
   std::mutex traffic_mutex;
   ```
2. Troque `float camera_x = 0, camera_z = 0;` por `std::atomic<float> camera_x{ 0 }, camera_z{ 0 };`
   (inclua `<atomic>`). Eles passam a ser lidos pela simulação (janela micro, C3).
3. Atualize o comentário do topo de `CityState` com o novo modelo de travas.

### B2. Buffer triplo de posições — novo arquivo `native/src/sim/state/pose_buffer.h`
Porte o desenho de `N/src/shared/contracts/poses.ts` (`PoseWriter`/`PoseReader`): sem espera, o escritor troca o
seu buffer pelo do meio, o leitor pega o do meio quando houver um novo.
```cpp
// Lock-free triple buffer of agent poses (roadcraft-novo shared/contracts/poses.ts): the simulation publishes
// after each tick, the render takes the newest without waiting. Each record carries the previous and current pose.
#pragma once
#include <atomic>
#include <cstdint>
#include <vector>
namespace city {
struct AgentRecord {
	float px, py, pz, pfx, pfz; // previous pose
	float x, y, z, fx, fz;      // current pose
	uint32_t id;                // vehicle or pedestrian id (wheel odometer, look)
	uint32_t look;              // pedestrians: look hash; vehicles: type | paint << 8 | brake << 16
	float walked;               // pedestrians: metres walked (walk phase); vehicles: 0
};
struct PoseFrame {
	std::vector<AgentRecord> vehicles, people;
	std::vector<float> seg_speed_share; // per segment: mean speed / limit (congestion layer), 1 when empty
	int64_t tick_ns = 0, period_ns = 0; // publication time and tick period (render interpolation)
};
class PoseBuffer {
public:
	PoseFrame &back() { return frames_[back_]; }
	void publish() { back_ = state_.exchange(back_ | NEW) & IDX; }
	// Returns the newest frame (and true when it changed since the last call).
	const PoseFrame &acquire(bool &fresh) {
		fresh = (state_.load() & NEW) != 0;
		if (fresh) front_ = state_.exchange(front_) & IDX;
		return frames_[front_];
	}
private:
	static constexpr int NEW = 4, IDX = 3;
	PoseFrame frames_[3];
	std::atomic<int> state_{ 1 }; // middle = 1
	int back_ = 0, front_ = 2;
};
} // namespace city
```
- Adicione `PoseBuffer poses;` em `CityState` (fora das travas: ele é sem trava por desenho).

### B3. Laço da simulação — `native/src/sim/simulation/simulation.cpp` e `.h`
1. **Relógio com prazo e recuperação** (corrige a perda de tempo sob carga): troque o corpo de `run()` por
   ```cpp
   int64_t deadline = now_ns();
   while (running_) {
   	deadline += period_ns_.load();
   	int64_t wait = deadline - now_ns();
   	if (wait > 0) std::this_thread::sleep_for(std::chrono::nanoseconds(wait));
   	else if (-wait > MAX_CATCHUP_TICKS * period_ns_.load()) deadline = now_ns(); // too far behind: drop, never spiral
   	int steps = steps_per_tick(speed_.load());
   	if (steps > 0 && s_->ready) tick(*s_, steps);
   	last_tick_ns_ = now_ns();
   }
   ```
   com `constexpr int MAX_CATCHUP_TICKS = 2;`.
2. Novo método privado `void tick(CityState &s, int steps)`:
   ```cpp
   const double dt = 1.0 / s.catalog.tune.sim_tick_hz;
   for (int k = 0; k < steps; k++) {
   	{
   		std::shared_lock map(s.map_mutex);
   		std::lock_guard tr(s.traffic_mutex);
   		traffic::step(s, dt);
   	}
   	{
   		std::unique_lock map(s.map_mutex, std::try_to_lock); // mesh jobs may hold it shared: never wait for them
   		s.clock.minutes.store(s.clock.minutes.load() + dt * s.catalog.tune.sim_minutes_per_second);
   		if (map.owns_lock()) { grow_step(s, owed_ + dt); owed_ = 0; } else owed_ += dt;
   		s.touch();
   	}
   }
   { std::shared_lock map(s.map_mutex); std::lock_guard tr(s.traffic_mutex); traffic::publish_poses(s); }
   ```
   - `owed_` é um novo campo `double owed_ = 0;` em `Simulation`: o tempo de crescimento não perdido quando a
     trava exclusiva não estava livre.
   - O relógio é escrito fora da trava quando ela não foi obtida. Torne `CityClock::minutes` um
     `std::atomic<double>` (em C++17 só há `load`/`store` para `atomic<double>`, sem `+=`) e ajuste leitores e
     escritores (`hour()`, `grow_step`, `save_write.cpp`, `save_read.cpp`, `world`/`city_gen`) para `.load()`/`.store()`.
   - O autosave continua intencional: o relógio é salvo, então a cidade sempre muda. **Não** mexa em
     `touch()`/`has_unsaved_work()`.
3. `step(CityState &s)` (usado por testes e por `CityCore::step_ticks`) passa a chamar `tick(s, 1)`.
   `CityCore::step_ticks` deixa de pegar trava (o `tick` pega as suas).
4. `interpolation()`: apague. A interpolação passa a usar `PoseFrame::tick_ns`/`period_ns` (B5).

### B4. Publicação das posições — `native/src/sim/traffic/traffic_system.cpp`
1. Tire `update_poses(s)` de dentro de `step()`. As poses passam a ser calculadas **uma vez por tick**, só para
   quem será publicado.
2. Nova função `void publish_poses(CityState &s)` (declare em `traffic_system.h`):
   - lê `cx = s.camera_x.load()`, `cz = s.camera_z.load()` e o raio de publicação `r = s.catalog.traffic.publish_radius_m`
     (nova chave em `data/catalog/traffic.json`, valor 900; schema e loader em `api/traffic_loader.cpp`);
   - `PoseFrame &f = s.poses.back(); f.vehicles.clear(); f.people.clear();`
   - para cada veículo vivo e **micro** (C3) dentro de `r` de (cx, cz): calcula a pose atual como hoje (`ends()` +
     o corpo entre frente e traseira; o código de `update_poses`), move `cur` para `prev`, grava `cur` e preenche
     um `AgentRecord` (`look = type | paint << 8 | brake << 16`);
   - para cada pedestre vivo dentro de `r`: o mesmo, com `look = w.look` e `walked = w.walked` (a pose do pedestre já
     é calculada em `walk()`/`pose()`; só copie `prev`/`cur`);
   - `seg_speed_share`: redimensione para `s.roads.segs.size()`; some a velocidade e o limite dos veículos micro por
     segmento (o laço de `OverlayRenderer::build_congestion` sai do render e vem para cá); os veículos meso
     contam com a velocidade estimada do meso (`len / (t_exit - t_in)`);
   - `f.tick_ns = now`, `f.period_ns = período do tick`; por fim, `s.poses.publish()`.
3. Apague `update_poses` (a lógica foi para `publish_poses`).

### B5. Render sem trava para agentes — `native/src/api/city_core.cpp`, `render/agents/*`, `render/render_world.h`
1. `CityCore::_process`:
   ```cpp
   s.camera_x.store(cam_.x); s.camera_z.store(cam_.z);
   bool fresh;
   const PoseFrame &f = state_->poses.acquire(fresh);
   float alpha = f.period_ns > 0 ? std::clamp(float(now_ns() - f.tick_ns) / float(f.period_ns), 0.f, 1.f) : 1.f;
   render_.update_agents(f, cam_, alpha);       // no lock
   render_.update_overlays(f, delta);           // congestion from f.seg_speed_share; no lock
   render_.sync(*state_, terrain3d(), cam_);    // B6: takes its own short locks
   ```
   Apague o `std::lock_guard` de `world_mutex` e a chamada a `full_rebuild` direta (B6 cuida disso).
2. `AgentRenderer::update(const PoseFrame &f, const Vector3 &cam, float alpha)` e
   `PeopleRenderer::update(const PoseFrame &f, ...)`: troque o laço sobre `s.traffic.vehicles`/`s.traffic.peds`
   pelo laço sobre `f.vehicles`/`f.people`. A interpolação usa `prev`/`cur` do registro. O `odo_` da roda continua
   indexado por `rec.id`. A fase de caminhada usa `rec.walked / pv.walk_advance`.
3. **Buffers persistentes** (`render/rs_util.h`, `agent_renderer.cpp`, `people_renderer.cpp`):
   - os `PackedFloat32Array` viram membros; conte as instâncias antes, faça **um** `resize(n * MM_STRIDE)` e escreva
     com um único `ptrw()`. Troque `mm_push` por `mm_write(float *w, ...)`, que escreve no ponteiro e retorna `w + MM_STRIDE`;
   - em `MultiMeshDraw::set`: aloque com folga (`capacity = round_up(count, 256)`) e só realoque quando
     `count > capacity` ou `count < capacity / 4`; use `multimesh_set_visible_instances(mm, count)`; aplique
     material, AABB e sombra **só quando mudarem** (guarde os últimos valores em campos).
4. `OverlayRenderer::build_congestion`: passe a ler `f.seg_speed_share` (não lê mais `s.traffic`).

### B6. Malhas em workers — novo `native/src/core/job_pool.h` e `render/render_world.cpp`
Porte o desenho de `N/src/jobs/pool.ts`: fila com prioridade, chave que substitui o pedido antigo da mesma chave
e resultado entregue à thread principal.
1. `core/job_pool.h` (C++ puro, sem Godot, camada `core`):
   ```cpp
   // Priority job pool (roadcraft-novo jobs/pool.ts): a fixed set of std::threads, jobs ordered by priority, a job
   // of the same key replaces a queued older one, results collected by the main thread with poll().
   class JobPool {
   public:
   	explicit JobPool(unsigned threads = std::max(2u, std::thread::hardware_concurrency() - 1));
   	~JobPool();
   	void submit(int64_t key, float priority, std::function<void()> work); // lower priority runs first
   	void parallel_for(size_t n, const std::function<void(size_t)> &fn);  // blocks until done (lot cache)
   };
   ```
   Implemente com `std::mutex` + `std::condition_variable` + `std::priority_queue`, e um
   `std::unordered_map<int64_t, uint64_t> latest` para descartar pedidos substituídos.
2. Cada renderer de chunk (`RoadRenderer`, `PropRenderer`, `TreeRenderer`, `BuildingRenderer`) ganha a separação
   **montar / enviar**:
   - `struct XxxBuilt { int64_t key; uint64_t seq; /* vetores de vértices, índices, instâncias por superfície */ };`
   - `XxxBuilt build_cpu(const CityState &s, int64_t key) const;` contém o código de montagem que hoje está em
     `rebuild_chunk`/`build` (`MeshBuilder`, `b40::Shell`, `place_trees` etc.), sem nenhuma chamada a `RenderingServer`;
   - `void commit(const XxxBuilt &b);` só faz `mesh_clear` + `mesh_add_surface_from_arrays`, multimesh e instância.
   - `MeshBuilder` e `b40::Shell` deixam de usar `Packed*Array` durante a montagem: passam a usar `std::vector`
     com `reserve` pela estimativa. A conversão para `Packed*Array` acontece **no worker**, no fim do `build_cpu`
     (`PackedArray` criado fora da thread principal é seguro; só o `RenderingServer` fica na principal).
   - `Shell::shaded`: tire os três `std::pow` + `macro_noise` por vértice; calcule a tonalidade por face e use uma
     tabela de gama de 256 entradas.
   - Estado mutável usado durante a montagem não pode ser compartilhado entre jobs. `BuildingBodies bodies_`
     (cache de corpos em `buildings/building_bodies.cpp`) vira `thread_local` dentro do `build_cpu`, e o `clear()` ao
     chegar em `CACHE_MAX` vira LRU (lista + mapa). Faça o mesmo com qualquer outro membro não-`const` lido pelos
     emissores (procure membros escritos dentro de `rebuild_chunk`/`build`/`place_trees`). Os dados de catálogo
     (`b40::Data`, tabelas) são só leitura e podem ser compartilhados.
   - `b40::Volume::bays` e `storey_paint` (`buildings/b40_types.h`) trocam `std::unordered_map` por vetores planos
     indexados por (andar, lado, bay).
3. `RenderWorld::sync`:
   - pega `std::unique_lock map(s.map_mutex)` **só** para drenar o `ChangeLog` para os conjuntos sujos de cada
     consumidor (B7) e copiar as chaves; solta a trava;
   - decide o que montar (streaming, B7/D1) e submete ao `JobPool` com prioridade = distância horizontal. Cada job
     faz `std::shared_lock map(s.map_mutex)` e chama `build_cpu`;
   - o resultado volta por uma fila de concluídos (mutex próprio). No quadro, a thread principal faz `commit` dos
     concluídos **por ordem de distância até gastar 4 ms** (`std::chrono::steady_clock`; nova chave de tuning
     `render.commit_budget_ms` = 4, substitui `max_chunk_rebuilds_per_frame`, que é apagada junto com
     `BUILDS_PER_FRAME`). Resultado com `seq` mais velho que o último pedido daquela chave é descartado;
   - `full_rebuild` vira "marcar todos os chunks conhecidos como sujos": não monta nada na thread principal.
4. `LotCache::process_dirty` (`sim/lots/lot_cache.cpp`): troque a criação de `std::thread` a cada chamada por
   `pool.parallel_for(cells.size(), ...)` do `JobPool` global (instância única em `CityState`, campo
   `std::unique_ptr<JobPool> jobs`). Abaixo de `LOT_SERIAL_MAX = 4` células, rode em série.
   `save_read.cpp` deixa de chamar `mark_all_lot_cells` + `process_dirty`: os lotes são calculados sob demanda em
   `LotCache::lots_of` (calcule e guarde no primeiro acesso a uma célula sem resultado).

### B7. Listas sujas por consumidor (desenho de `N/src/core/grid.ts`, `DirtyReader`)
1. `ChangeLog` continua sendo o canal único dos produtores (as quatro listas atuais).
2. `RenderWorld` ganha um `std::unordered_set<int64_t>` por consumidor: `roads_dirty_`, `props_dirty_`,
   `trees_dirty_`, `buildings_dirty_`, `overlay_dirty_`. O dreno (B6.3) copia `road_chunks` para roads/props/trees/overlay,
   `building_chunks` para buildings/trees, e limpa o `ChangeLog`. O conjunto elimina os duplicados.
3. `TreeRenderer`: `dirty_` vira o conjunto acima, sem reconstrução imediata. Chunks sujos entram nos jobs com
   orçamento como os outros, e só se monta o nível (perto/longe) que está na faixa visível.
4. Quando `render_ok_` é falso (testes sem tela), `CityCore::_process` drena e descarta o `ChangeLog`, para que ele
   não cresça sem limite.

### B8. Comandos e consultas — `native/src/api/*.cpp`
Troque cada `std::lock_guard<std::mutex> lk(state_->world_mutex)`:
- **exclusiva** (`std::unique_lock lk(state_->map_mutex)`): `city_core_commands.cpp` (build_road, demolish,
  paint_zone), `city_core_edit.cpp` (undo, redo, strokes, split, upgrade, roundabout, terrain_brush,
  draw_building, set_building), `city_core.cpp` (new_map, mark_saved), `city_core_persist.cpp` (load_city);
- **compartilhada** (`std::shared_lock lk(state_->map_mutex)`): `raycast`, `inspect`, `probe_building`,
  `list_buildings`, `city_bounds`, `snap_road`, `preview_road`, `preview_roundabout`, `preview_lot`,
  `segment_at`, `road_profiles`, `constants`, `snapshot`, e o `encode_city` de `save_city`;
- `traffic_probe`: `std::shared_lock map` + `std::lock_guard tr(state_->traffic_mutex)`.

### B9. Consulta de altura e câmera
1. `CityCore::height_at(float x, float z) -> float`: `std::shared_lock map` + `return state_->terrain.height_at(x, z);`.
   Faça o bind em `city_core_bind.cpp` e adicione `&"height_at": "height_at"` em `QUERIES` de
   `project/game/store/city_store.gd`.
2. `project/game/camera/city_camera.gd`, `_ground_height`: troque o raycast por
   `return CityStore.query(&"height_at", [x, z])`. Em `_apply`, chame `_ground_height` uma vez para o foco e uma para
   o olho (como hoje), mas guarde o último par (x, z) → altura e reaproveite quando o ponto não mudou mais que 1 cm.
3. `project/game/tools/tool_controller.gd`: `ground_at` passa a ser chamado no máximo uma vez por quadro (guarde o
   resultado do quadro em um campo e reaproveite).

### B10. Snapshot barato — `city_core_query.cpp` e `city_store.gd`
1. Em `CityState`, um contador `uint64_t edit_revision` incrementado só pelos comandos (em `finish_edit` e no
   undo/redo), não pelo tick.
2. `snapshot()` guarda em cache as contagens caras (ocupação dos prédios, nós, lados zonados) e só as recalcula
   quando `edit_revision` mudou. Relógio, dinheiro e `unsaved` são lidos sempre.
3. `city_store.gd`: depois de um comando, em vez de `refresh()` imediato, marque `_refresh_pending = true` e faça
   um único `refresh()` no próximo `_process`. Leia `DataTables.game("saves").autosave_s` uma vez em `_ready`.

### B11. Save em thread — `city_core_persist.cpp`
1. `save_city(path)`: sob `std::shared_lock map`, faça `encode_city` (cópia em memória). Solte a trava e grave
   com ZSTD numa `std::thread` destacada que escreve em `path + ".tmp"` e depois renomeia
   (`DirAccess::rename_absolute`). O resultado vai para um campo atômico `save_state_` (0 ocioso, 1 gravando,
   2 ok, 3 erro). `saved_revision` é atualizado quando termina ok.
2. Novo `save_status() -> Dictionary` (`{busy, ok, error, bytes}`), com bind e entrada em `QUERIES`.
3. `city_store.gd`: o autosave chama `save_game` e não espera; o sinal `saved` é emitido quando `save_status`
   mostrar o fim (verifique em `_process`). O salvar manual do menu pode esperar `busy == false` antes de mostrar
   a mensagem.

---

## Bloco C — Simulação com a arquitetura do novo

### C1. Arena de rotas — `sim/traffic/traffic_state.h` e `routing.cpp`
1. Em `Vehicle`, troque `std::vector<int> route; int ri;` por `int route_off = -1, route_len = 0, route_cap = 0, ri = 0;`.
2. Em `TrafficState`: `std::vector<int> route_pool; size_t route_waste = 0;`, com funções em novo
   `sim/traffic/route_arena.cpp`: `int route_at(const TrafficState&, const Vehicle&, int k)`,
   `void route_set(TrafficState&, Vehicle&, const int *src, int n)` (reusa o espaço se `n <= route_cap`; senão anexa
   no fim do pool e soma o antigo a `route_waste`) e `void route_compact(TrafficState&)`, que roda no início do
   `step` quando `route_waste > route_pool.size() / 2` e reescreve as rotas vivas em ordem.
3. Troque todos os usos de `v.route[...]`, `v.route.size()`, `v.route.back()` e `splice()` nos arquivos
   `routing.cpp`, `demand.cpp`, `vehicle_motion.cpp`, `junction_control.cpp`, `junction_admission.cpp` e
   `lane_change.cpp` pelas funções acima. `splice` monta a rota nova num `thread_local std::vector<int>` reutilizado
   e chama `route_set`. `find_route` escreve no `thread_local` do chamador (nada de vetor novo por chamada).
4. O mesmo vale para `Pedestrian::route` (`ped_route_off/len/cap` em `ped_pool`).

### C2. Grafo de faixas incremental — `sim/traffic/lane_graph.cpp` (desenho de `N/src/sim/lanes.ts`)
O novo refaz só os nós sujos (`dirtyNodes`, `rebuildNode`, `clearNode`), com as conexões numa tabela com lista
livre e listas ligadas por nó (`nodeConn`/`nextAtNode`) e por faixa de origem (`laneConn`/`nextFromLane`).
1. **Quem suja.** Os únicos mutadores das vias são as operações `j_*` de `sim/commands/journal_ops.cpp`. Em cada
   uma que toca a rede (adicionar/remover nó, adicionar/remover/trocar perfil de segmento, controle de nó), anexe
   ao `ChangeLog` (B7) novos campos: `std::vector<int> net_nodes` (nós sujos: o nó, ou os dois nós do segmento) e
   `std::vector<int> net_segs_removed` / `net_segs_changed`. Esses campos **não** são drenados pelo render: quem
   drena é o tráfego.
2. **Estruturas.** Em `LaneGraph`:
   - `Conn` ganha `int next_at_node = -1, next_from = -1; bool alive = false;`; `conns` vira um pool com
     `std::vector<int> free_conns`;
   - `std::vector<int> node_conn` (primeira conexão de cada nó, -1) e `std::vector<int> lane_conn` (primeira conexão
     que sai de cada `ds * MAX_LANES + lane`, -1), no lugar de `ds_out`. Ache todos os usos com
     `grep -rn "ds_out" native/src` e troque cada laço `for (int ci : g.ds_out[ds])` por um laço sobre as listas
     `lane_conn` de cada faixa do `ds` (seguindo `next_from`). Pule conexões com `alive == false`;
   - `conflicts` e `conflict_at` continuam por conexão.
3. **Funções** (substituem `build_lane_graph` no caminho de edição; `build_lane_graph` inteiro fica só para mapa
   novo e carga):
   - `void build_dir(const CityState&, LaneGraph&, int seg)`: o corpo de `build_dirs` para um segmento (os dois `ds`),
     atualizando `lane_metres` pela diferença;
   - `void clear_node(CityState&, int n)`: percorre `node_conn[n]` e, para cada conexão, chama
     `on_conn_removed(s, c)` (os veículos nela vão para o início da faixa de destino se ela existir, senão são
     despachados com `despawn_vehicle(..., false)`), retira-a de `lane_conn` da faixa de origem, limpa conflitos, devolve
     a `free_conns`, zera `node_conn[n]` e a junção do nó;
   - `void build_node(...)`: o código atual, mas alocando conexões do pool e encadeando em `node_conn`/`lane_conn`,
     com `find_conflicts` só entre as conexões desse nó (como hoje);
   - `void apply_network_changes(CityState &s)`, chamada no início de `traffic::step` no lugar de
     `if (t.g.version != s.roads.version) rebuild(s);`:
     1. para cada segmento removido: despache os veículos das suas faixas (`lane_vehicles` dos dois `ds`) e do meso
        (C3), e zere os `ds`;
     2. para cada segmento alterado ou novo: `build_dir` (redimensione `lane_vehicles`, `lane_conn`, `exit_claim` e
        demais vetores por faixa quando o número de segmentos crescer);
     3. para cada nó sujo (sem repetir): `clear_node` e, se o nó estiver vivo, `build_node` + `build_junction(s, n)`
        (extraia de `build_junctions` a parte de um nó);
     4. os pedestres nos segmentos removidos são despachados; os outros continuam.
   - Apague a função `rebuild()` de `traffic_system.cpp` (que apaga a frota). Mapa novo e carga chamam
     `build_lane_graph` + `clear_traffic(s)` explicitamente em `CityCore::new_map`/`load_city`.
4. **Rota que ficou inválida** (porte de `N/src/sim/traffic/meso.ts` `advance()` e do micro): quando `pick_connector`
   não acha conexão da faixa atual para `route[ri+1]`, tente outra faixa do mesmo `ds` que faça a curva
   (troca de faixa); se não houver, replaneje com `find_route(ds atual → destino)`; se falhar, despache.

### C3. Meso no mapa todo + janela micro (porte de `N/src/sim/traffic/meso.ts` e da janela de `N/src/sim/traffic.ts`)
1. **Dados.** Em `Vehicle`: `bool micro = true; int ml = -1, mnext = -1, mprev = -1; double t_in = 0, t_exit = 0; float s0 = 0;`.
   Em `TrafficState`: `std::vector<int> meso_head, meso_tail` (por `ds * MAX_LANES + lane`), `std::vector<double> lane_last_exit`,
   heap de eventos (`std::vector<double> ev_t; std::vector<int> ev_v;`) e `std::vector<int> micro_list` (ids micro;
   índice em `Vehicle::micro_idx`).
2. **Arquivo novo `sim/traffic/meso.cpp`**: porte linha a linha das funções `link`, `unlink`, `inWindow`, `enter`,
   `process`, `advance`, `schedule` e `popEvent` de `meso.ts`, com as constantes `MESO_ALPHA = 0.95`,
   `MESO_CROWD = 12`, `S0 = 2.2`, `T_HEAD = 0.9`, `DWELL = 14`, que vão para `traffic.json` como `meso_alpha`, `meso_crowd`,
   `meso_s0_m`, `meso_head_s` e `meso_dwell_s`. Adaptações:
   - lane do novo (`seg*16 + dir*8 + idx`) → aqui `ds * MAX_LANES + lane`;
   - `h.router.nodeCost(...)` → `junction_cost` de `routing.cpp` (exponha);
   - `h.sig.go(node, arm)` / `h.sig.nextGreen(...)` → `light_for(s, ds)` (C5) e o tempo até o próximo verde calculado
     do ciclo de `Junction` (`green`, `amber`, `cycle`, `offset`);
   - ônibus, linhas e passagem de nível ficam para o bloco E (deixe os ramos com a mesma estrutura, chamando
     funções vazias que E preenche);
   - `h.admit`, `h.lastAdmit`, `h.releaseAt`, `reconsider` → as funções que já existem em `junction_admission.cpp`
     e `routing.cpp`.
3. **Janela.** Constantes `meso_window_m = 1500` e `meso_window_out_m = 1750` em `traffic.json` (valores do novo).
   Porte `checkWindow` e `materialize` de `traffic.ts` para `sim/traffic/window.cpp`:
   - rode `check_window(s)` no início do `step` quando a câmera andou mais de `meso_window_step_m = 64` desde a
     última vez, ou a cada `meso_window_every_s = 1` s;
   - veículo micro cuja faixa saiu do raio `window_out` (e que não está numa conexão) → `remove_micro` + `meso_enter(spawned=true)`;
   - segmentos dentro do raio `window` com fila meso → `materialize(lane)`, que põe os veículos na posição estimada
     pelo tempo, atrás dos que já estão desenhados (código de `materialize` do novo);
   - a consulta de segmentos perto da câmera usa um índice espacial de segmentos pelo ponto médio (se `RoadNetwork`
     não tiver, crie `SpatialHash seg_index` com célula de 256 m, atualizado nas operações `j_*`).
4. **Passo.** Em `traffic::step`, a ordem passa a ser: `apply_network_changes` → `route_compact` (se preciso) →
   `check_window` → `step_governor` → `spawn_vehicles` → `meso_process` → `change_lanes` → `step_vehicles` →
   `step_pedestrians`.
5. **Laços que crescem com o mapa** (proibidos):
   - `step_vehicles`: troque o laço sobre todos os `ds`/faixas por um laço sobre `active_lanes`, um vetor de chaves de
     faixa com pelo menos um veículo micro, mantido nas funções que põem e tiram veículos de `lane_vehicles`
     (`leave_lane`, a inserção do spawn, a saída da conexão e o `materialize`). Use uma marca
     `std::vector<uint8_t> lane_active` para não repetir e retire a faixa da lista quando ela esvaziar. Conexões: `active_conns`, do mesmo jeito;
   - o laço de "preso há muito tempo" (`stuck_despawn_s`) e o de `change_lanes` passam a percorrer `micro_list`, não
     todos os slots de veículo.
6. **Spawn** (`demand.cpp`, `spawn_one`):
   - **antes** de rotear, cheque `lane_free` e o limite da faixa (hoje estão depois do `find_route`); mova as duas
     linhas para cima do `find_route`;
   - limite de expansão do spawn: nova chave `spawn_route_max_expanded = 4000`, passada a `find_route`;
   - nasce micro só se a faixa estiver a menos de `micro_spawn_m = 350` m da câmera (valor do novo); senão nasce meso
     (`meso_enter(v, lane, true)`);
   - o alvo da frota continua `lane_metres * densidade * fator do dia` (o meso aguenta o mapa todo).
7. **Pedestres**: passam a existir só perto da câmera. O alvo usa os metros de calçada dentro de
   `ped_window_m = 600` (nova chave); nascimento só dentro desse raio; quem sai de `ped_window_m * 1.2` é removido
   sem contar como perdido. Isso dá o teto natural da multidão (hoje não há teto).
   - Pedestre em `WAIT`: tente atravessar a 5 Hz (contador por pedestre), não a cada passo.
   - `walk_ok` e `cars_on_crossing` por (nó, braço) ficam em cache com o número do tick (`stamp`).

### C4. Rotas mais baratas — `routing.cpp`, `astar.h`
1. Ao montar ou alterar um `ds` (C2), grave o ponto final em `std::vector<float> ds_end_x, ds_end_z` do `LaneGraph`.
   A heurística vira `std::hypot(ds_end_x[ds] - gx, ds_end_z[ds] - gz) / guess` (sem `lane_pose` por nó aberto).
2. `astar.h`: guarde `g` no próprio heap junto com `f` (evite recalcular no pop).

### C5. Cruzamentos sem trabalho repetido — `junction_control.cpp`, `vehicle_motion.cpp`, `junction_admission.cpp`
1. No início do `step`, para cada nó com semáforo **dentro da janela micro** (os de fora o meso consulta pelo
   relógio), calcule o estágio atual uma vez: `std::vector<int16_t> stage_now` e `std::vector<uint8_t> light_now`
   por `ds`. `light_for(s, ds)` passa a ler `light_now[ds]`.
2. Em `step_lane`: `gate()`, `may_enter()` e `peds_in_way()` rodam **só para a cabeça da faixa** (`list.back()`).
   Os demais seguem o líder pelo IDM (como já fazem quando não são cabeça).
3. `has_exit`: mantenha `std::vector<int> exit_load` por chave de faixa de saída (incrementado/decrementado em
   `insert`/`leave_lane`/entrada e saída de conexão) e consulte-o, em vez de varrer as conexões do nó.
4. `pick_connector`: guarde o resultado em `Vehicle::pick_cache` com `pick_ri` e `pick_lane`; recalcule só quando
   `ri` ou a faixa mudarem.
5. `idm_accel`: troque `std::pow(x, IDM_DELTA)` por `x*x*x*x` (delta = 4). `desired_speed`: troque o `std::sin` da
   deriva por uma tabela de 256 amostras.

### C6. Crescimento — `sim/simulation/city_growth.cpp`
1. `GrowthQueue::push`: use `std::unordered_set<int64_t> members` junto do vetor (sem `std::find`).
2. Células vazias: remoção por troca com o último (`swap` + `pop_back`), não `erase` no meio.
3. `nearest_free`: mantenha por célula um cursor do próximo lote livre (`std::unordered_map<int64_t, int> cursor`) e
   não percorra a fila inteira. Escolha a célula mais perto do último prédio entre as `growth_scan = 16` primeiras
   da fila (nova chave).

### C7. Undo e modificadores de terreno
1. `journal_ops.cpp`: `record()` soma `sizeof(EditOp)` + `capacity()` dos vetores internos do op (e a tabela de
   arco do segmento), não a constante `OP_BYTES = 64`. `EditHistory::trim` soma também o `redo_stack`.
2. `road_edit.cpp`, `conform_under`: guarde o id do modificador criado em `RoadSegment::mod_id` (novo campo,
   salvo no arquivo: suba `SAVE_VERSION`, leia com valor -1 em versões antigas). Ao demolir o segmento
   (`road_commands.cpp`), chame `j_remove_modifier(s, mod_id)` (crie a op, com inverso, em `journal_ops.cpp`).
   `change_profile_at` (`road_tabs.cpp`): pule segmentos que já estão no perfil-alvo e substitua o modificador do
   segmento em vez de criar outro.

---

## Bloco D — Renderização com os recursos do Godot (regras do novo)

### D1. Streaming pela distância no chão — `render/chunk_queue.h`, `buildings/building_renderer.cpp`, `trees/tree_renderer.cpp`
1. `ChunkStream::distance(k, cx, cy, cz)` → `distance(k, cx, cz)`: só `dx`, `dz` (tire `cy`). Ajuste as chamadas em
   `render_world.cpp`.
2. `BuildingRenderer::update_view`: use a distância horizontal do centro do chunk à câmera para **decidir o que
   montar**. As faixas de visibilidade (`ranges`) continuam pela distância do olho, que é a medida certa de
   tamanho na tela.
3. O último nível (o HLOD de D2) fica com `end = 0` (sem fim): a cidade nunca some ao afastar.
4. `update_view` não percorre mais todos os `chunks_`: percorre só as células dentro do alcance (como
   `ChunkStream::update`) e os chunks montados.

### D2. HLOD por superchunk — `buildings/building_renderer.cpp/.h` (regra de `N/src/render/cellLayer.ts`)
1. O nível `far` deixa de ser por chunk de 256 m. Crie `struct Super { MeshDraw far; bool dirty; }` em
   `std::unordered_map<int64_t, Super> supers_`, com a chave `chunk_of(x, z, SUPERCHUNK)` (a constante já existe em
   `core/grid.h`).
2. O job de um superchunk percorre os 16 chunks dele e emite `b40::emit_far` de todos os prédios num único `Shell`
   (sem sombra: `shadows=false`). Faixa: `[mid_end, 0]`.
3. Um chunk de prédios sujo marca o superchunk dele como sujo.
4. Material do far: `building_far` (D5).

### D3. Peças dos prédios instanciadas com corte por tamanho na tela (regra de `N/src/render/partSize.ts`)
Todas as peças de fachada passam por `Shell::box(Basis, x0..x1, y0..y1, z0..z1, lin, fin, skip)` (veja `unit_box`
em `buildings/part_models.cpp`). Em vez de gerar triângulos para o `Shell` de peças, gere **instâncias de uma
caixa unitária**:
1. `b40::Shell` ganha `bool instanced = false;` e `std::vector<BoxInstance> boxes;`, com
   `struct BoxInstance { float basis[9]; float origin[3]; uint32_t color; int8_t fin; float thick; };`. Em
   `Shell::box`, quando `instanced`, calcule o centro e as meias-medidas da caixa no referencial do `Basis`, grave
   colunas = eixos × medidas e origem = centro, e `thick` = **a segunda maior medida** (a regra do novo: barra pela
   sua largura).
2. `BuildingRenderer::build` (nível perto) cria o `Shell parts` com `instanced = true`. O que não for caixa (lâmpada,
   AC, toldo) continua como geometria num `Shell` comum.
3. Separe as instâncias em **5 faixas por espessura**: ≤ 0,08 m, ≤ 0,2 m, ≤ 0,5 m, ≤ 1,5 m e maior. Cada faixa vira
   um `MultiMeshDraw` por chunk, com a malha de uma caixa unitária criada uma vez. O fim da faixa de visibilidade
   de cada uma é a distância em que a peça mais grossa dela ocupa `px_min` pixels na tela:
   `end = thick_max * screen_h / (2 * tan(fov/2) * px_min)`, com `screen_h = 1080`, `fov` = `far_fov_deg` de
   `camera.json` e `px_min = 0.5` para a primeira faixa (barras) e 1 para as outras (valores de `BAR_PIXELS` e
   `PART_ERROR_PX` do novo). Grave os 5 limites e os `px_min` em `data/render/buildings.json` (`lod.part_bands`).
4. Material das instâncias: `building_parts` (D5), que lê `fin` e cor de `INSTANCE_CUSTOM`/`COLOR` em vez do
   atributo de vértice.
5. Peças não projetam sombra (já é assim) e o detalhe de lote segue a mesma regra: em `buildings/lot_emit.cpp`,
   troque a cerca de uma caixa a cada 0,14 m por um painel único com a textura repetida (UV ao longo do comprimento),
   e o piso tessellado de 3 m por um quad por elemento.

### D4. Vértices menores — `buildings/shell.cpp`, `mesh_builder.cpp`
1. Em `commit_to`, passe `RenderingServer::ARRAY_FLAG_COMPRESS_ATTRIBUTES` como flag do
   `mesh_add_surface_from_arrays` (existe na API 4.7).
2. Não envie `ARRAY_TEX_UV2` quando o material da camada não a lê. Veja em cada `.gdshader` se `UV2` é usado;
   onde não for, não preencha.
3. Índices: se o chunk tiver menos de 65.536 vértices, o Godot já usa 16 bits. Garanta isso dividindo superfícies
   maiores por sub-chunk de 128 m (`SUBCHUNK`).

### D5. Materiais por nível — `project/render/city_world.gd` e `project/render/shaders/`
1. `building_far.gdshader`: cópia de `building.gdshader` sem `window()` (interior mapping), sem `weather()` e sem
   `finish()`. A cor vem só do vértice e o vidro é uma cor chapada escurecida com o céu. Material `building_far` em
   `CityWorld.materials()` e na lista de materiais lida por `BuildingRenderer::init` (`ctx.material("building_far", ...)`).
2. `building_parts.gdshader`: cópia de `building.gdshader` em que o `vertex()` lê `fin` e tom de
   `INSTANCE_CUSTOM`/`COLOR` (o MultiMesh usa `use_colors` e `use_custom_data`, que `MultiMeshDraw` já liga).
3. `terrain.gdshader`: troque os hashes com `sin` por hash inteiro (`pcg`, como em `building.gdshader`) e corte o
   micro-ruído e a segunda amostra rotacionada da grama além de 300 m (`smoothstep` pela distância).
4. `water.gdshader`: em `ripple()`, retorne cedo quando `fine <= 0.0`.
5. `road.gdshader`: passe as médias das texturas (`mean_of_asphalt` e afins) como uniformes calculadas uma vez em
   `city_world.gd` e só calcule `lamp_pools()` quando `night > 0.0`.

### D6. Sombras
1. Nível perto e mid projetam sombra; far/HLOD e peças não.
2. `city_world.gd`: `SHADOW_PARALLEL_3_SPLITS`, com a distância máxima vinda do preset gráfico (D8).

### D7. Árvores e camadas
1. `TreeRenderer::build_cpu`: consulte vias e lotes **uma vez por chunk** (listas com os segmentos e lotes que
   tocam o chunk + 80 m) e teste os candidatos contra essas listas, não uma consulta espacial por candidato.
2. Monte só o nível (perto ou longe) cuja faixa toca o alcance atual.
3. `OverlayRenderer` (zonas): malha por chunk de 256 m, montada em job quando o chunk está em `overlay_dirty_` e a
   camada está ligada. Congestionamento: por chunk, a partir de `f.seg_speed_share` (B4), refeito a cada `refresh_s`
   só para os chunks perto da câmera (alcance de vias).

### D8. Texturas e presets gráficos
1. Nos 17 arquivos `project/assets/surfaces/*.png.import` com `importer="texture"`: `compress/mode=2`; nos que
   terminam em `.normal.png` e `.detail-normal.png`, também `compress/normal_map=1`; em todos, `detect_3d/compress_to=0`.
   Nos 5 com `importer="2d_array_texture"` (`building-finishes.*`, `terrain-land.*`): `compress/mode=2`.
   Apague a pasta `project/.godot/imported` para forçar a reimportação.
2. `project/project.godot`: apague `anti_aliasing/quality/msaa_3d=2`.
3. `project/game/play.gd`, depois de `add_child(world)`: `Settings.apply(world, get_viewport())`.
4. `project/data/game/settings.json`: cada preset ganha `shadow_splits` e o alto/ultra ganham TAA
   (`viewport.use_taa`). Leia esses campos em `Settings.apply`.

---

## Bloco E — Recursos do novo que faltam no Godot

Para cada linha: leia o(s) `.ts` de origem inteiro(s), porte a lógica pura para C++ no destino seguindo a tabela de
tradução da seção 0, exponha os comandos/consultas em `api/city_core_*.cpp` + `city_core_bind.cpp` +
`COMMANDS`/`QUERIES` de `city_store.gd`, e ligue a interface na gaveta/aba correspondente de `project/ui` (dados em
`data/ui/drawers.json`, textos em `strings.csv`). Toda mutação passa por uma op `j_*` com inverso
(`journal_ops.cpp`), para ter desfazer e refazer.

| # | Recurso | Origem em N | Destino em G |
|---|---|---|---|
| E1 | Economia das vias (preço por m² × modo de construção, devolução de 25 %, recusa sem saldo, cidade gerada grátis) | `src/roads/economy.ts` | `sim/commands/road_commands.cpp` (já cobra: confira as fórmulas contra o .ts) + `api` |
| E2 | Grade de via: ponte, viaduto, túnel, corte com talude/muro | `src/roads/construction/grade.ts`, `execute.ts`, `plan.ts` | `sim/commands/road_plan.cpp`, `road_commands.cpp`, `road_edit.cpp` (altura do segmento) + `render/roads/road_segment.cpp` (tabuleiro/pilares já existem; boca de túnel e muro novos) |
| E3 | Menu principal e tela inicial (semente, tamanho do mapa, novo/abrir/salvar) | `src/ui/` (tela inicial e menu) | novo `project/ui/menus/main_menu.gd/.tscn`; `GameFlow` com BOOT → MAIN_MENU → LOADING → PLAYING; `play.gd` passa a receber mapa/semente de `GameFlow.pending` |
| E4 | Edições de via: mover nó, unir, inverter, duplicar, retorno, transição de largura, vagas por lado | `src/roads/construction/segmentEdits.ts`, `streetEdits.ts` | `sim/commands/road_tabs.cpp` (dividir em arquivos novos por edição) |
| E5 | Controle de cruzamento por perna, fases, movimentos proibidos, conectores manuais, automático pelo fluxo | `src/roads/control.ts`, `src/roads/laneLinks.ts`, `src/sim/signals.ts` | `sim/traffic/junction_control.cpp` + campos em `RoadNode` + ops `j_*` |
| E6 | Placa = regra, nós impossíveis | `src/roads/signRule.ts`, `src/roads/impossible.ts` | `sim/roads/` (arquivos novos) + `api` |
| E7 | Nomes e numeração de rua | `src/roads/streetNames.ts` | novo `sim/roads/street_names.cpp` + `inspector_panel.gd` |
| E8 | Ônibus, linhas, paradas, terminais, trem e metrô | `src/sim/traffic/buses.ts`, `lines.ts`, `stops.ts`, `kerbStops.ts`, `src/world/transit.ts`, `src/features/transit/transitTool.ts` | novos `sim/traffic/buses.cpp`, `lines.cpp`, `stops.cpp`; ferramenta `project/game/tools/transit_tool.gd`; preencher os ramos de ônibus deixados no meso (C3) |
| E9 | Manobras (vaga, doca de ré, meio-fio) | `src/sim/reedsShepp.ts`, `src/sim/manoeuvres.ts` | novos `sim/traffic/reeds_shepp.cpp`, `manoeuvres.cpp` (só dentro de 350 m da câmera, como no novo) |
| E10 | Passagem de nível com cancela | ramos `Ctl.Crossing` de `meso.ts`/`traffic.ts` | `junction_control.cpp` + meso |
| E11 | Lote em polígono, dividir por linha, curvar lado, desgaste e reforma, endereço | `src/world/lotEdit.ts`, `src/world/decay.ts` | `sim/commands/land_commands.cpp` (dividir) + `sim/buildings/` |
| E12 | Crescimento como no novo (sequência, 3 andares acima do vizinho, praça por bairro) | `src/world/growth.ts` | `sim/simulation/city_growth.cpp`, `sim/buildings/growth.cpp` |
| E13 | Mobiliário e itens (muros, cercas, sebes, defensas, bancos etc.) | `src/world/items.ts`, `src/features/streetscape/itemTool.ts` | novo `sim/items/` + `render/props/` + ferramenta GDScript |
| E14 | Construtor de prédios (abas Modelos, Massa, Fachada, Peças, Telhado, Interior, Cores) e os 38 marcos | `src/world/design.ts`, `catalog*.ts`, `src/ui/creator.ts`, `data/lots-roadcraft` | `sim/commands/land_commands.cpp` (set_building) + `render/buildings/*` + abas em `project/ui` |
| E15 | Rio e água | `src/features/terrain/riverTool.ts`, `src/features/water/waterControl.ts` | `sim/terrain/` + `terrain_brush.cpp` + ferramenta |
| E16 | Demolição com bomba e ruínas | `src/features/demolition/**` | novo `render/demolition/` + comando |
| E17 | Céu, tempo, nuvens, neblina | `src/features/sky/*` | `project/render/daylight.gd`, `city_world.gd` (WorldEnvironment) |

---

## Bloco F — Gates e documento único

1. **Mapa do `CLAUDE.md`**: gere a lista de arquivos mapeados (raízes em `tools/gates/rules.json` → `mapped_roots`)
   e a primeira linha do comentário de cabeçalho de cada um com um script Python temporário (fora do projeto), e
   reescreva a seção entre `<!-- MAPA:INICIO -->` e `<!-- MAPA:FIM -->` agrupada por pasta, no formato atual
   (`` `caminho` `` + responsabilidade). Diretórios de muitos arquivos iguais podem usar `` `pasta/**` ``. O
   `CLAUDE.md` precisa ficar com no máximo 330 linhas.
2. No `CLAUDE.md`, atualize: "Fluxo de dados e threads" (B1–B7: `map_mutex`/`traffic_mutex`, buffer triplo,
   `JobPool`, commit com 4 ms), "Máquinas de estado" (GameFlow com menu), "Pendências" (só o que ficou de fora).
3. `tools/gates/rules.json` → `cpp_layers`: adicione as pastas `sim/citygen`, `sim/lots` e as novas (`sim/items`, ...)
   com as mesmas regras de `sim/*`. Permita que `sim/*` inclua `sim/state` e outras pastas `sim/*` (a regra do
   projeto é `core → sim/* → render → api`). Isso resolve `CAMADA` e `CAMADA_DESCONHECIDA`.
4. Rode `python3 tools/gates/run_gates.py` e corrija **todos** os problemas:
   - `NUMERO_MAGICO` → constante nomeada no dono ou chave de dados;
   - `DADOS`/`DADOS_SEM_SCHEMA` → atualize os schemas em `tools/gates/schemas/` para as chaves reais (inclusive as
     novas deste roteiro);
   - `TAMANHO_*` → divida (`road_profiles.json` e `strings.csv` em arquivos por assunto; o `DataTables` lê todos);
   - `CODIGO_DUPLICADO`, `SEGUNDO_DONO`, `VALOR_DUPLICADO`, `DONO_AUSENTE`, `ORFAO`, `TUNING_SEM_USO`, `SEM_LIXO`,
     `FLUXO_CENTRAL` → siga a mensagem de cada um (o `play.gd` passa a sair e trocar de tela só pelo `GameFlow`).
   Termine com "Gates OK".

---

## Bloco G — Compilar e entregar

1. `tools/build/build.sh core` e `tools/build/build.sh core-windows` (ou os comandos cmake equivalentes no Windows).
   Corrija **todos** os erros e avisos de compilação de uma vez e recompile até zerar.
2. `python3 tools/gates/run_gates.py` → "Gates OK".
3. `tools/build/build.sh export && tools/build/build.sh package`.
4. Relatório final: lista dos blocos com os commits, o que foi portado de cada arquivo de `N`, o que ficou em
   Pendências e o que não foi compilado ou visto (diga exatamente, sem afirmar o que não aconteceu).
