import { Injectable } from '@angular/core';
import { HttpClient } from '@angular/common/http';
import { BehaviorSubject, Observable, map, catchError, of, tap } from 'rxjs';

export interface ScheduleItem {
  id: number;
  time: string;
  active: boolean;
}

@Injectable({
  providedIn: 'root',
})
export class FoodScheduleService {
  // IP del ESP32
  private apiUrl = 'http://192.168.0.54/api';

  // BehaviorSubject para datos reactivos
  private schedulesSubject = new BehaviorSubject<ScheduleItem[]>([]);

  // Observable público
  public schedules$ = this.schedulesSubject.asObservable();

  constructor(private http: HttpClient) {
    this.loadSchedules();
  }

  private loadSchedules(): void {
    this.http
      .get<any[]>(`${this.apiUrl}/schedules`)
      .pipe(
        map((schedules) =>
          schedules.map((s) => ({
            id: s.id,
            time: s.time,
            active: s.active,
          }))
        ),
        tap((schedules) => {
          this.schedulesSubject.next(schedules);
        }),
        catchError(() => {
          console.warn('No se pudo conectar con el ESP32, usando datos vacíos');
          return of([]);
        })
      )
      .subscribe();
  }

  // Obtener todos los horarios
  getSchedules(): ScheduleItem[] {
    return this.schedulesSubject.value;
  }

  // Crear un nuevo horario
  createSchedule(schedule: Omit<ScheduleItem, 'id'>): ScheduleItem {
    const newSchedule = {
      time: schedule.time,
      active: schedule.active,
    };

    this.http
      .post<{ message: string; id: number }>(`${this.apiUrl}/schedules`, newSchedule)
      .pipe(
        catchError(() => {
          console.error('Error al crear horario');
          return of(null);
        })
      )
      .subscribe((response) => {
        if (response) {
          this.loadSchedules();
        }
      });

    // Retornar temporalmente
    return { ...schedule, id: 0 };
  }

  // Actualizar un horario existente
  updateSchedule(id: number, schedule: Partial<ScheduleItem>): ScheduleItem | null {
    const updateData = {
      id,
      ...(schedule.time && { time: schedule.time }),
      ...(schedule.active !== undefined && { active: schedule.active }),
    };

    this.http
      .put(`${this.apiUrl}/schedules/update`, updateData)
      .pipe(
        catchError(() => {
          console.error('Error al actualizar horario');
          return of(null);
        })
      )
      .subscribe(() => {
        this.loadSchedules();
      });

    const schedules = this.schedulesSubject.value;
    const index = schedules.findIndex((s) => s.id === id);
    if (index !== -1) {
      schedules[index] = { ...schedules[index], ...schedule };
      return schedules[index];
    }
    return null;
  }

  // Eliminar un horario
  deleteSchedule(id: number): boolean {
    this.http
      .request('delete', `${this.apiUrl}/schedules/delete`, {
        body: { id },
      })
      .pipe(
        catchError(() => {
          console.error('Error al eliminar horario');
          return of(null);
        })
      )
      .subscribe(() => {
        this.loadSchedules();
      });

    const schedules = this.schedulesSubject.value;
    const index = schedules.findIndex((s) => s.id === id);
    if (index !== -1) {
      schedules.splice(index, 1);
      this.schedulesSubject.next([...schedules]);
      return true;
    }
    return false;
  }
}
