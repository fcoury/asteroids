use raylib::prelude::*;

use crate::utils::rand_float;

mod utils;

const SCREEN_WIDTH: i32 = 800;
const SCREEN_HEIGHT: i32 = 450;
const STARTING_ASTEROIDS: i32 = 4;
const ASTEROID_SPEED_MIN: f32 = 80.0;
const ASTEROID_SPEED_MAX: f32 = 180.0;
const ASTEROID_LARGE_RADIUS: f32 = 50.0;
const ASTEROID_MEDIUM_RADIUS: f32 = 25.0;
const ASTEROID_SMALL_RADIUS: f32 = 8.0;
const ROT_SPEED: f32 = 5.0;
const SHIP_HEIGHT: f32 = 30.0;
const SHIP_WIDTH: f32 = 15.0;
const SHIP_ACCELERATION: f32 = 200.0; // pixels per second squared
const SHIP_MAX_VELOCITY: f32 = 400.0;
const DRAG: f32 = 0.5;

struct Ship {
    pos: Vector2,
    velocity: Vector2,
    rot: f32,
}

impl Default for Ship {
    fn default() -> Self {
        Self {
            pos: Vector2::new(SCREEN_WIDTH as f32 / 2.0, SCREEN_HEIGHT as f32 / 2.0),
            velocity: Vector2::new(0.0, 0.0),
            rot: 0.0,
        }
    }
}

enum AsteroidSize {
    Large,
    Medium,
    Small,
}
impl AsteroidSize {
    fn radius(&self) -> f32 {
        match self {
            Self::Large => ASTEROID_LARGE_RADIUS,
            Self::Medium => ASTEROID_MEDIUM_RADIUS,
            Self::Small => ASTEROID_SMALL_RADIUS,
        }
    }
}

struct Asteroid {
    pos: Vector2,
    velocity: Vector2,
    rot: f32,
    rot_speed: f32,
    size: AsteroidSize,
    active: bool,
}

impl Asteroid {
    fn radius(&self) -> f32 {
        self.size.radius()
    }
}

struct Bullet {
    pos: Vector2,
    velocity: Vector2,
    rot: f32,
    active: bool,
}

#[derive(Default, Debug)]
enum GameState {
    #[default]
    Ready,
    Playing,
    GameOver,
}

#[derive(Default)]
struct Game {
    ship: Ship,
    asteroids: Vec<Asteroid>,
    bullets: Vec<Bullet>,
    wave: i32,
    state: GameState,
}

impl Game {
    fn update(&mut self, d: &mut RaylibDrawHandle<'_>) {
        let dt = d.get_frame_time();
        let forward = Vector2::new(self.ship.rot.cos(), self.ship.rot.sin());

        match self.state {
            GameState::GameOver | GameState::Ready if d.is_key_pressed(KeyboardKey::KEY_SPACE) => {
                println!("starting");
                self.spawn_wave(d);
                self.state = GameState::Playing;
            }
            GameState::Playing if d.is_key_pressed(KeyboardKey::KEY_SPACE) => self.fire(),
            GameState::Ready => return,
            _ => (),
        };

        if d.is_key_down(KeyboardKey::KEY_LEFT) || d.is_key_down(KeyboardKey::KEY_H) {
            self.ship.rot -= ROT_SPEED * dt;
        }

        if d.is_key_down(KeyboardKey::KEY_RIGHT) || d.is_key_down(KeyboardKey::KEY_L) {
            self.ship.rot += ROT_SPEED * dt;
        }

        if d.is_key_down(KeyboardKey::KEY_DOWN) || d.is_key_down(KeyboardKey::KEY_J) {
            let acceleration = forward.scale(SHIP_ACCELERATION);
            self.ship.velocity = self.ship.velocity + acceleration.scale(dt);
        }

        self.ship.velocity = self.ship.velocity.scale((-DRAG * dt).exp());

        if self.ship.velocity.length_sqr() < 0.01 {
            self.ship.velocity = Vector2::new(0.0, 0.0);
        }

        self.ship.pos += self.ship.velocity.scale(dt);

        for a in self.asteroids.iter_mut().filter(|a| a.active) {
            println!("pos before: {:?}", a.pos);
            a.pos = wrap_position(a.pos + a.velocity.scale(dt));
            println!("pos after: {:?}", a.pos);
            a.rot = a.rot_speed * dt;
        }

        for b in self.bullets.iter_mut().filter(|b| b.active) {
            b.pos += b.velocity.scale(dt);

            if b.pos.x < 0.0
                || b.pos.x > SCREEN_WIDTH as f32
                || b.pos.y < 0.0
                || b.pos.y > SCREEN_HEIGHT as f32
            {
                b.active = false;
            }
        }
    }

    fn draw(&mut self, d: &mut RaylibDrawHandle<'_>) {
        d.clear_background(Color::RAYWHITE);

        match self.state {
            GameState::Ready => {
                d.draw_text(
                    "Press SPACE to start",
                    SCREEN_WIDTH / 2 - 100,
                    SCREEN_HEIGHT / 2,
                    20,
                    Color::BLACK,
                );
            }
            GameState::Playing => {
                self.draw_ship(d);
            }
            GameState::GameOver => {
                d.draw_text(
                    "Game Over! Press ENTER to restart",
                    SCREEN_WIDTH / 2 - 150,
                    SCREEN_HEIGHT / 2,
                    20,
                    Color::WHITE,
                );
            }
        }

        for a in self.asteroids.iter().filter(|a| a.active) {
            d.draw_circle_lines(a.pos.x as i32, a.pos.y as i32, a.radius(), Color::BLUE);
        }
    }

    fn spawn_wave(&mut self, d: &mut RaylibDrawHandle<'_>) {
        self.wave += 1;

        let count = STARTING_ASTEROIDS + (self.wave - 1);
        for _ in 0..count {
            let pos = if d.get_random_value::<i32>(0..=1) == 0 {
                Vector2::new(
                    if d.get_random_value::<i32>(0..=1) == 0 {
                        0.0
                    } else {
                        SCREEN_HEIGHT as f32
                    },
                    rand_float(d, 0.0, SCREEN_WIDTH as f32),
                )
            } else {
                Vector2::new(
                    rand_float(d, 0.0, SCREEN_HEIGHT as f32),
                    if d.get_random_value::<i32>(0..=1) == 0 {
                        0.0
                    } else {
                        SCREEN_WIDTH as f32
                    },
                )
            };
            self.spawn_asteroid(d, pos, AsteroidSize::Large);
        }
    }

    fn spawn_asteroid(&mut self, d: &mut RaylibDrawHandle<'_>, pos: Vector2, size: AsteroidSize) {
        let angle = rand_float(d, 0.0, 2.0 * PI as f32);
        let speed = rand_float(d, ASTEROID_SPEED_MIN, ASTEROID_SPEED_MAX);

        self.asteroids.push(Asteroid {
            pos,
            size,
            velocity: Vector2::new(angle.cos() * speed, angle.sin() * speed),
            rot: rand_float(d, 0.0, 2.0 * PI as f32),
            rot_speed: rand_float(d, -2.0, 2.0),
            active: true,
        });
    }

    fn fire(&self) {
        todo!()
    }

    fn draw_ship(&self, d: &mut RaylibDrawHandle<'_>) {
        // draw ship
        let ship_points = [
            Vector2::new(0.0, -SHIP_HEIGHT / 2.),
            Vector2::new(-SHIP_WIDTH / 2., SHIP_HEIGHT / 2.),
            Vector2::new(SHIP_WIDTH / 2., SHIP_HEIGHT / 2.),
        ];
        let ship_rotated: Vec<Vector2> = ship_points
            .iter()
            .map(|p| {
                let x = p.x * self.ship.rot.cos() - p.y * self.ship.rot.sin();
                let y = p.x * self.ship.rot.sin() + p.y * self.ship.rot.cos();
                Vector2::new(x + self.ship.pos.x, y + self.ship.pos.y)
            })
            .collect();
        d.draw_triangle_lines(
            ship_rotated[0],
            ship_rotated[1],
            ship_rotated[2],
            Color::BLACK,
        );
    }
}

fn wrap_position(pos: Vector2) -> Vector2 {
    let mut res = pos.clone();

    if pos.x < 0.0 {
        res.x += SCREEN_WIDTH as f32;
    }
    if pos.x > SCREEN_WIDTH as f32 {
        res.x -= SCREEN_WIDTH as f32;
    }
    if pos.y < 0.0 {
        res.y += SCREEN_HEIGHT as f32;
    }
    if pos.y > SCREEN_HEIGHT as f32 {
        res.y -= SCREEN_HEIGHT as f32;
    }

    res
}

fn main() {
    let (mut rl, thread) = raylib::init()
        .size(SCREEN_WIDTH, SCREEN_HEIGHT)
        .title("Asteroidz")
        .build();

    rl.set_target_fps(60);

    let mut g = Game::default();

    while !rl.window_should_close() {
        let mut d = rl.begin_drawing(&thread);
        g.update(&mut d);
        g.draw(&mut d);
    }
}
