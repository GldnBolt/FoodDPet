import { Injectable } from '@angular/core';

export interface ScheduleItem {
  id: number;
  time: string;
  amount: number;
  active: boolean;
}

@Injectable({
  providedIn: 'root',
})
export class FoodScheduleService {
  private schedules: ScheduleItem[] = [
    { id: 1, time: '08:00', amount: 100, active: true },
    { id: 2, time: '14:00', amount: 150, active: true },
    { id: 3, time: '20:00', amount: 100, active: true },
  ];

  private nextId = 4;

  // Obtener todos los horarios
  getSchedules(): ScheduleItem[] {
    // TODO: Reemplazar con llamada al microcontrolador
    return [...this.schedules];
  }

  // Crear un nuevo horario
  createSchedule(schedule: Omit<ScheduleItem, 'id'>): ScheduleItem {
    // TODO: Reemplazar con llamada al microcontrolador
    const newSchedule: ScheduleItem = {
      ...schedule,
      id: this.nextId++,
    };
    this.schedules.push(newSchedule);
    return newSchedule;
  }

  // Actualizar un horario existente
  updateSchedule(id: number, schedule: Partial<ScheduleItem>): ScheduleItem | null {
    // TODO: Reemplazar con llamada al microcontrolador
    const index = this.schedules.findIndex((s) => s.id === id);
    if (index !== -1) {
      this.schedules[index] = { ...this.schedules[index], ...schedule };
      return this.schedules[index];
    }
    return null;
  }

  // Eliminar un horario
  deleteSchedule(id: number): boolean {
    // TODO: Reemplazar con llamada al microcontrolador
    const index = this.schedules.findIndex((s) => s.id === id);
    if (index !== -1) {
      this.schedules.splice(index, 1);
      return true;
    }
    return false;
  }
}
