import { Component, OnInit, OnDestroy } from '@angular/core';
import { CardModule } from 'primeng/card';
import { DashboardService, FoodRequest } from '../service/dashboard-service';
import { TableModule } from 'primeng/table';
import { CommonModule } from '@angular/common';
import { TagModule } from 'primeng/tag';
import { ButtonModule } from 'primeng/button';
import { InputTextModule } from 'primeng/inputtext';
import { FormsModule } from '@angular/forms';
import { Subscription } from 'rxjs';

@Component({
  selector: 'app-dashboard',
  imports: [
    CardModule,
    TableModule,
    CommonModule,
    TagModule,
    ButtonModule,
    InputTextModule,
    FormsModule,
  ],
  templateUrl: './dashboard.html',
})
export class Dashboard implements OnInit, OnDestroy {
  fullnessStatus: string = 'Cargando...';
  fullnessPercentage: number = 0;
  requestHistory: FoodRequest[] = [];
  isRefilling: boolean = false;
  esp32IpInput: string = '';

  private subscriptions: Subscription[] = [];

  constructor(private dashboardService: DashboardService) {}

  ngOnInit() {
    // Cargar IP actual del servicio
    this.esp32IpInput = this.dashboardService.getEsp32Ip();

    // Suscribirse a los observables para recibir actualizaciones
    this.subscriptions.push(
      this.dashboardService.fullnessStatus$.subscribe((status) => {
        this.fullnessStatus = status;
      })
    );

    this.subscriptions.push(
      this.dashboardService.percentFullness$.subscribe((percentage) => {
        this.fullnessPercentage = percentage;
      })
    );

    this.subscriptions.push(
      this.dashboardService.requestHistory$.subscribe((history) => {
        this.requestHistory = history;
      })
    );

    this.subscriptions.push(
      this.dashboardService.esp32Ip$.subscribe((ip) => {
        this.esp32IpInput = ip;
      })
    );
  }

  ngOnDestroy() {
    // Limpiar suscripciones
    this.subscriptions.forEach((sub) => sub.unsubscribe());
  }

  updateEsp32Ip() {
    if (this.esp32IpInput.trim()) {
      this.dashboardService.setEsp32Ip(this.esp32IpInput.trim());
      alert('IP actualizada exitosamente');
    } else {
      alert('Por favor ingresa una IP válida');
    }
  }

  refillDispenser() {
    this.isRefilling = true;
    this.dashboardService.refillDispenser().subscribe({
      next: () => {
        this.isRefilling = false;
        alert('¡Dispensador rellenado exitosamente!');
      },
      error: () => {
        this.isRefilling = false;
        alert('Error al rellenar el dispensador');
      },
    });
  }

  completeRequest(request: FoodRequest) {
    this.dashboardService.completeRequest(request);
  }

  cancelRequest(request: FoodRequest) {
    this.dashboardService.cancelRequest(request);
  }
}
