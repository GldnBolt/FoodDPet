import { Component, OnInit } from '@angular/core';
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
export class FoodSchedule implements OnInit {
  schedules: ScheduleItem[] = [];
  displayDialog = false;
  isEditMode = false;
  selectedSchedule: ScheduleItem = this.getEmptySchedule();

  constructor(private foodScheduleService: FoodScheduleService) {}

  ngOnInit() {
    this.loadSchedules();
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
        amount: this.selectedSchedule.amount,
        active: this.selectedSchedule.active,
      });
    }
    this.loadSchedules();
    this.displayDialog = false;
  }

  deleteSchedule(schedule: ScheduleItem) {
    if (confirm(`¿Está seguro que desea eliminar el horario de ${schedule.time}?`)) {
      this.foodScheduleService.deleteSchedule(schedule.id);
      this.loadSchedules();
    }
  }

  toggleActive(schedule: ScheduleItem) {
    this.foodScheduleService.updateSchedule(schedule.id, { active: !schedule.active });
    this.loadSchedules();
  }

  private getEmptySchedule(): ScheduleItem {
    return {
      id: 0,
      time: '',
      amount: 100,
      active: true,
    };
  }
}
