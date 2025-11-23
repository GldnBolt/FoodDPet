import { Component, OnInit, OnDestroy } from '@angular/core';
import { CommonModule } from '@angular/common';
import { FormsModule } from '@angular/forms';
import { TableModule } from 'primeng/table';
import { ButtonModule } from 'primeng/button';
import { DialogModule } from 'primeng/dialog';
import { InputTextModule } from 'primeng/inputtext';
import { InputNumberModule } from 'primeng/inputnumber';
import { CardModule } from 'primeng/card';
import { TagModule } from 'primeng/tag';
import { TooltipModule } from 'primeng/tooltip';
import { FoodScheduleService, ScheduleItem } from '../service/food-schedule.service';
import { Subscription } from 'rxjs';

@Component({
  selector: 'app-food-schedule',
  imports: [
    CommonModule,
    FormsModule,
    TableModule,
    ButtonModule,
    DialogModule,
    InputTextModule,
    InputNumberModule,
    CardModule,
    TagModule,
    TooltipModule,
  ],
  templateUrl: './food-schedule.html',
})
export class FoodSchedule implements OnInit, OnDestroy {
  schedules: ScheduleItem[] = [];
  displayDialog = false;
  isEditMode = false;
  selectedSchedule: ScheduleItem = this.getEmptySchedule();

  private subscription?: Subscription;

  constructor(private foodScheduleService: FoodScheduleService) {}

  ngOnInit() {
    // Suscribirse a los cambios de horarios
    this.subscription = this.foodScheduleService.schedules$.subscribe((schedules) => {
      this.schedules = schedules;
    });
  }

  ngOnDestroy() {
    // Limpiar suscripción
    this.subscription?.unsubscribe();
  }

  loadSchedules() {
    this.schedules = this.foodScheduleService.getSchedules();
  }

  openNewDialog() {
    this.selectedSchedule = this.getEmptySchedule();
    this.isEditMode = false;
    this.displayDialog = true;
  }

  openEditDialog(schedule: ScheduleItem) {
    this.selectedSchedule = { ...schedule };
    this.isEditMode = true;
    this.displayDialog = true;
  }

  saveSchedule() {
    if (this.isEditMode) {
      this.foodScheduleService.updateSchedule(this.selectedSchedule.id, this.selectedSchedule);
    } else {
      this.foodScheduleService.createSchedule({
        time: this.selectedSchedule.time,
        active: this.selectedSchedule.active,
      });
    }
    this.displayDialog = false;
  }

  deleteSchedule(schedule: ScheduleItem) {
    if (confirm(`¿Está seguro que desea eliminar el horario de ${schedule.time}?`)) {
      this.foodScheduleService.deleteSchedule(schedule.id);
    }
  }

  toggleActive(schedule: ScheduleItem) {
    this.foodScheduleService.updateSchedule(schedule.id, { active: !schedule.active });
  }

  private getEmptySchedule(): ScheduleItem {
    return {
      id: 0,
      time: '',
      active: true,
    };
  }
}
